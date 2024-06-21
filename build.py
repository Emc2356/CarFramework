#!/usr/bin/env python

from typing import TextIO, Callable, Any

from collections import defaultdict, deque

from pathlib import Path

import subprocess
import shutil
import json
import sys
import os


class Logger:
    verbose: bool = False

    @classmethod
    def info(cls, message: str) -> None:
        if not cls.verbose:
            return
        print(f"[INFO] {message}", file=sys.stdout)

    @classmethod
    def warn(cls, message: str) -> None:
        print(f"[WARNING] {message}", file=sys.stderr)

    @classmethod
    def error(cls, message: str, code: int=1) -> None:
        print(f"\033[31m[ERROR] {message}\033[0m", file=sys.stderr)
        if code != 0:
            sys.exit(code)


class LogFile:
    data: dict[str, float] = {}
    log_file: Path = Path.cwd() / "build" / "log.log"
    is_loaded: bool = False

    @classmethod
    def update(cls, file) -> None:
        if not cls.is_loaded:
            cls.load()
            cls.is_loaded = True
        cls.set(str(file), os.path.getmtime(str(file)))

    @classmethod
    def set(cls, name: str, time: float) -> None:
        if not cls.is_loaded:
            cls.load()
            cls.is_loaded = True
        cls.data[name] = time
        cls.save()

    @classmethod
    def get(cls, name: str) -> float:
        if not cls.is_loaded:
            cls.load()
            cls.is_loaded = True
        return cls.data.get(name, 0)

    @classmethod
    def load(cls) -> None:
        cls.log_file.touch(exist_ok=True)

        lines = cls.log_file.read_text().splitlines()

        if len(lines) % 2 == 1:
            cls.data = {}

        for i, j in zip(range(0, len(lines), 2), range(1, len(lines), 2)):
            cls.data[lines[i]] = float(lines[j])

    @classmethod
    def save(cls) -> None:
        with cls.log_file.open("w") as f:
            for name, time in cls.data.items():
                f.write(f"{name}\n{time}\n")


class SourceFile:
    __slots__ = ("path", "watches")

    def __init__(self, path: str, watches: list=[]) -> None:
        self.path: Path = Path(path)
        self.watches: list[SourceFile] = SourceFile.from_list(watches)

        if not self.path.exists():
            Logger.error(f"file `{str(self.path)}` does not exist")

    def __str__(self) -> str:
        return str(self.path)

    def __repr__(self) -> str:
        return str(self.path)

    def read(self) -> str:
        return self.path.read_text()

    def write(self, data: str) -> None:
        self.path.write_text(data)

    def exists(self) -> bool:
        return self.path.exists()

    def was_modified(self) -> bool:
        return LogFile.get(str(self.path)) != os.path.getmtime(str(self.path))

    def object_file_upto_date(self) -> bool:
        return not self.was_modified() and self.to_object_filename().exists() and all(x.was_modified() for x in self.watches)

    def has_suffx(self, suffix) -> bool:
        return str(self.path).endswith(suffix)

    def to_object_filename(self) -> Path:
        build_dir = get_build_directory()
        ret = get_build_directory()

        for s in Path(self.path).parts:
            if s in build_dir.parts:
                continue
            ret /= s

        return ret.with_suffix(".o")

    @staticmethod
    def from_list(objects: list) -> list["SourceFile"]:
        ret: list[SourceFile] = []
        for obj in objects:
            if isinstance(obj, str):
                ret.append(SourceFile(obj))
            elif isinstance(obj, SourceFile):
                ret.append(obj)
            else:
                Logger.error(f"unrecognized object {obj}")

        return ret


class Compiler:
    toolchain: str = "GNU"
    c_compiler: str = "gcc"
    cxx_compiler: str = "g++"
    linker: str = "g++"
    c_standard: int = 17
    cxx_standard: int = 17
    build_flags: list[str] = []
    link_flags: list[str] = []
    defines: dict[str, str | None] = {}

    @classmethod
    def set_toolchain(cls, toolchain: str) -> None:
        if toolchain.lower() == "gnu":
            cls.toolchain = "GNU"
            cls.c_compiler = "gcc"
            cls.cxx_compiler = "g++"
            cls.linker = "g++"
        elif toolchain.lower() == "clang":
            cls.toolchain = "clang"
            cls.c_compiler = "clang"
            cls.cxx_compiler = "clang"
            cls.linker = "clang"
            exit("TODO")
        else:
            Logger.error(f"toolchain `{toolchain}` is not supported")

    @classmethod
    def set_cxx_standard(cls, standard: int) -> None:
        cls.cxx_standard = standard

    @classmethod
    def set_c_standard(cls, standard: int) -> None:
        cls.c_standard = standard

    @classmethod
    def add_define(cls, name: str, value: str | None=None) -> None:
        if name in cls.defines.keys():
            Logger.warn(f"redefining `{name}` macro")
        cls.defines[name] = str(value) if value is not None else None

    @classmethod
    def add_build_flags(cls, *args) -> None:
        for arg in args:
            if isinstance(arg, str):
                cls.build_flags.append(arg)
            elif isinstance(arg, (list, tuple)):
                cls.add_build_flags(arg)
            else:
                Logger.error(f"Expected a series of strings `{arg}`")

    @classmethod
    def add_link_flags(cls, *args) -> None:
        for arg in args:
            if isinstance(arg, str):
                cls.link_flags.append(arg)
            elif isinstance(arg, (list, tuple)):
                cls.add_link_flags(arg)
            else:
                Logger.error(f"Expected a series of strings `{arg}`")


    @classmethod
    def construct_build_command_gnu_clang(cls, other: "StaticLibrary | Executable | PreCompiledHeader") -> tuple[list[str], list[str]]:
        command_c: list[str] = [
            cls.c_compiler,
            f"-std=gnu{cls.c_standard}",
        ]
        command_cxx: list[str] = [
            cls.cxx_compiler,
            f"-std=c++{cls.cxx_standard}",
        ]

        common: list[str] = ["-c"] + other.extra_build_flags + cls.build_flags

        for name, value in cls.defines.items():
            common.append("-D")

            if value is None:
                common.append(name)
            else:
                common.append(f"{name}={value}")

        for define in other.extra_defines:
            name = define[0]
            value = define[1] if len(define) > 1 else None

            common.append("-D")

            if value is None:
                common.append(name)
            else:
                common.append(f"{name}={value}")

        if isinstance(other, (StaticLibrary, Executable)):
            for include_directory in other.include_directories:
                common.append("-I")
                common.append(include_directory)

        command_c.extend(common)
        command_cxx.extend(common)


        for precompiled_header in PreCompiledHeader.get():
            command_cxx.append("-include")
            command_cxx.append(f"{precompiled_header.source}")

            if precompiled_header.source.has_suffx(".h"):
                command_c.append("-include")
                command_c.append(f"{precompiled_header.source}")

        return command_c, command_cxx

    @classmethod
    def construct_build_command(cls, other: "StaticLibrary | Executable | PreCompiledHeader") -> tuple[list[str], list[str]]:
        if cls.toolchain == "GNU":
            return cls.construct_build_command_gnu_clang(other)
        else:
            Logger.error(f"Internal error at construct_build_command, should have failed earlier")


class PreCompiledHeader:
    _registered: list["PreCompiledHeader"] = []

    __slots__ = ("source", "extra_build_flags", "extra_defines")

    def __init__(self, source: str | SourceFile, extra_build_flags: list[str], extra_defines: list[tuple[str, str] | tuple[str]]) -> None:
        self.source: SourceFile = SourceFile.from_list([source])[0]
        self.extra_build_flags: list[str] = extra_build_flags
        self.extra_defines: list[tuple[str, str] | tuple[str]] = extra_defines

        PreCompiledHeader._registered.append(self)

    @classmethod
    def get(cls) -> list["PreCompiledHeader"]:
        return cls._registered

    @classmethod
    def build_all(cls) -> None:
        for header in cls._registered:
            c_build_command, cxx_build_command = Compiler.construct_build_command(header)

            Logger.info(f"generating command for {header.source}")
            if header.source.has_suffx(".hpp"):
                build_command = cxx_build_command
            elif header.source.has_suffx(".h"):
                build_command = c_build_command
            else:
                Logger.warn(f"unrecognized file extension `{header.source.path.suffix}` for {header.source}, assuming C++")
                build_command = cxx_build_command

            if header.source.was_modified() or not Path(str(header.source) + ".gch").exists():
                CommandQueue.add(
                    build_command + [str(header.source)],
                    f"compiling {header.source}",
                    f"failed to compile {header.source}"
                )

                LogFile.update(header.source)
            else:
                Logger.info(f"`{header.source}` already up to date")

        CommandQueue.flush()



class StaticLibrary:
    _registered: list["StaticLibrary"] = []

    __slots__ = ("name", "out_filepath", "sources", "extra_build_flags", "extra_defines", "depends_on", "include_directories")
    def __init__(self, name: str, out_filepath: str, sources: list[str | SourceFile],
                 extra_build_flags: list[str]=[], extra_defines: list[tuple[str, str] | tuple[str]]=[],
                 depends_on: list[str]=[], include_directories: list[str]=[]):
        self.name: str = name
        self.out_filepath: Path = Path(out_filepath)
        self.sources: list[SourceFile] = SourceFile.from_list(sources)
        self.extra_build_flags: list[str] = extra_build_flags
        self.extra_defines: list[tuple[str, str] | tuple[str]] = extra_defines
        self.depends_on: list[str] = depends_on
        self.include_directories: list[str] = include_directories

        StaticLibrary._registered.append(self)

    @classmethod
    def get(cls) -> list["StaticLibrary"]:
        return cls._registered

    # returns true if anything was rebuild
    @classmethod
    def build_all(cls) -> bool:
        needs_linking: bool = False

        for library in cls._registered:
            c_build_command, cxx_build_command = Compiler.construct_build_command(library)

            object_files_in_static_library: list[Path] = []
            needs_rebuilding: bool = False
            for source_file in library.sources:
                if source_file.has_suffx(".cpp"):
                    build_command = cxx_build_command
                elif source_file.has_suffx(".c"):
                    build_command = c_build_command
                else:
                    Logger.warn(f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                    build_command = cxx_build_command

                object_file = source_file.to_object_filename()

                # ensure that the compiler doesnt fail because the parent directory doesnt exist
                object_file.parent.mkdir(parents=True, exist_ok=True)

                # check if the object file is up to date
                if not source_file.object_file_upto_date():
                    CommandQueue.add(
                        build_command + ["-o", str(object_file), str(source_file)],
                        f"compiling {source_file}",
                        f"failed to compile {source_file}"
                    )

                    # save the time
                    LogFile.update(str(source_file))
                    needs_rebuilding = True
                    needs_linking = True
                else:
                    Logger.info(f"`{source_file}` already up to date")

                object_files_in_static_library.append(object_file)

            # make sure that the object files exist before the library is created
            CommandQueue.flush()
            if needs_rebuilding or not (library.out_filepath / f"lib{library.name}.a").exists():
                library.out_filepath.mkdir(parents=True, exist_ok=True)
                CommandQueue.add(
                    ["ar", "rcs", "-o", str(library.out_filepath / f"lib{library.name}.a")] + list(str(path) for path in object_files_in_static_library),
                    f"creating static library {library.out_filepath / f"lib{library.name}.a"}",
                    f"failed to create static library {library.out_filepath / f"lib{library.name}.a"}"
                )
                CommandQueue.flush()
                needs_linking = True

        return needs_linking


class Executable:
    _registered: "Executable"

    __slots__ = (
        "name", "sources", "extra_build_flags", "extra_link_flags",
        "include_directories", "library_directories", "libraries", "extra_defines"
    )
    def __init__(self, name: str, sources: list[str | SourceFile],
                 extra_build_flags: list[str]=[], extra_link_flags: list[str]=[], extra_defines: list[tuple[str] | tuple[str, str]]=[],
                 include_directories: list[str]=[], library_directories: list[str]=[], libraries: list[str]=[]):
        self.name: str = name
        self.sources: list[SourceFile] = SourceFile.from_list(sources)
        self.extra_build_flags: list[str] = extra_build_flags
        self.extra_link_flags: list[str] = extra_link_flags
        self.include_directories: list[str] = include_directories
        self.library_directories: list[str] = library_directories
        self.libraries: list[str] = libraries
        self.extra_defines: list[tuple[str] | tuple[str, str]] = extra_defines

        Executable._registered = self

    @classmethod
    def get(cls) -> "Executable":
        return cls._registered

    # returns true if anything was built
    @classmethod
    def build(cls) -> bool:
        c_build_command, cxx_build_command = Compiler.construct_build_command(cls.get())

        needs_linking: bool = False

        for source_file in cls.get().sources:
            Logger.info(f"{source_file}")

            Logger.info(f"generating command for {source_file}")
            if source_file.has_suffx(".cpp"):
                build_command = cxx_build_command
            elif source_file.has_suffx(".c"):
                build_command = c_build_command
            else:
                Logger.warn(f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                build_command = cxx_build_command

            object_file = source_file.to_object_filename()

            object_file.parent.mkdir(parents=True, exist_ok=True)

            if not source_file.object_file_upto_date():
                CommandQueue.add(
                    build_command + ["-o", str(object_file), str(source_file)],
                    f"compiling {source_file}",
                    f"failed to compile {source_file}"
                )

                LogFile.update(source_file)
                needs_linking = True
            else:
                Logger.info(f"`{source_file}` already up to date")

        CommandQueue.flush()

        return needs_linking

    @classmethod
    def link(cls) -> None:
        command: list[str] = [Compiler.linker] + Compiler.link_flags + Executable.get().extra_link_flags

        static_library_directories: set = set()
        for static_library in StaticLibrary.get():
            static_library_directories.add(static_library.out_filepath)

        for dir in static_library_directories:
            command.append("-L")
            command.append(str(dir))

        command.append("-o")
        command.append(Executable.get().name)

        for source_file in Executable.get().sources:
            command.append(str(source_file.to_object_filename()))

        for lib in Executable.get().libraries:
            command.append("-l")
            command.append(f"{lib}")


        __libraries = list(lib.name for lib in StaticLibrary.get())
        __dependencies = {lib.name: lib.depends_on for lib in StaticLibrary.get()}

        # Create a graph
        __graph = defaultdict(list)
        __in_degree = {lib: 0 for lib in __libraries}

        # Build the graph and in-degree count
        for lib, deps in __dependencies.items():
            for dep in deps:
                __graph[dep].append(lib)
                __in_degree[lib] += 1

        # Find all nodes with no incoming edges
        queue = deque([lib for lib in __libraries if __in_degree[lib] == 0])

        sorted_static_libraries_names = []

        while queue:
            lib = queue.popleft()
            sorted_static_libraries_names.append(lib)

            # Decrease the in-degree of the neighboring nodes
            for neighbor in __graph[lib]:
                __in_degree[neighbor] -= 1
                if __in_degree[neighbor] == 0:
                    queue.append(neighbor)

        # Check if there was a cycle
        if len(sorted_static_libraries_names) != len(__libraries):
            Logger.error("Cycle detected in dependencies")

        sorted_static_libraries = []
        for name in sorted_static_libraries_names:
            for lib in StaticLibrary.get():
                if name == lib.name:
                    sorted_static_libraries.append(lib)

        for static_library in sorted_static_libraries[::-1]:
            command.append("-l")
            command.append(f"{str(static_library.name)}")

        CommandQueue.add(
            command,
            f"linking objects into `{Executable.get().name}`",
            "linker failed"
        )

        CommandQueue.flush()


class CommandQueue:
    size: int = 1
    queue: list[list[str]] = []
    messages: list[str] = []
    fail_messages: list[str] = []

    @classmethod
    def add(cls, command: list[str], message: str, fail_message: str) -> None:
        cls.queue.append(command)
        cls.messages.append(message)
        cls.fail_messages.append(fail_message)

        if cls.size == -1:
            return

        if len(cls.queue) >= cls.size:
            cls.flush()

    @classmethod
    def flush(cls) -> None:
        if len(cls.queue) == 0:
            return

        processes: list[subprocess.Popen] = []

        for i in range(len(cls.queue)):
            Logger.info(cls.messages[i])
            processes.append(
                subprocess.Popen(
                    cls.queue.pop(0)
                )
            )

        returncodes = [process.wait() for process in processes]
        for i, returncode in enumerate(returncodes):
            if returncode != 0:
                Logger.error(cls.fail_messages[i])

        cls.queue.clear()
        cls.messages.clear()
        cls.fail_messages.clear()


################################################################
##################### GENERAL FUNCTION #########################
################################################################
def consume_arg(argv: list[str], argument: str | list[str]) -> bool:
    if isinstance(argument, list):
        for arg in argument:
            if consume_arg(argv, arg):
                return True
    elif isinstance(argument, str):
        for arg in argv:
            if arg == argument:
                argv.remove(argument)
                return True
    else:
        Logger.error(f"internal error: :consume_arg({argv}, {argument}):")
    return False


def usage(program_name: str, out: TextIO) -> None:
    print(f"{program_name} Usage:", file=out)
    print("    -h, --help display this message", file=out)
    print("    --verbose show info messages", file=out)
    print("    -j<number> specify the amount of commands it can run simultaneously, if no number is then it will be set to use 75% of your cores", file=out)
    print("    --full compiles the project and generates the compile_commands.json for clangd", file=out)
    print("    --clangd generates the compile_commands.json file for clangd extension", file=out)
    print("    --force force rebuilds everything", file=out)
    print("    --clean cleans everything produced by this script, default is false", file=out)


################################################################
####################### BUILD FUNCTIONS ########################
################################################################
def get_build_directory() -> Path:
    ret = get_root_dir() / "build"
    ret.mkdir(parents=True, exist_ok=True)
    return ret


def generate_clangd_commands() -> None:
    result = []

    for static_library in StaticLibrary.get():

        c_build_command, cxx_build_command = Compiler.construct_build_command(static_library)

        for source_file in static_library.sources:
            Logger.info(f"generating command for {source_file}")
            if source_file.has_suffx(".cpp"):
                build_command = cxx_build_command
            elif source_file.has_suffx(".c"):
                build_command = c_build_command
            else:
                Logger.warn(f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                build_command = cxx_build_command
            result.append({
                "directory": str(get_root_dir()),
                "arguments": build_command + ["-o", str(source_file.to_object_filename())],
                "file": str(source_file),
            })


    c_build_command, cxx_build_command = Compiler.construct_build_command(Executable.get())

    for source_file in Executable.get().sources:
        if source_file.has_suffx(".cpp"):
            build_command = cxx_build_command
        elif source_file.has_suffx(".c"):
            build_command = c_build_command
        else:
            Logger.warn(f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
            build_command = cxx_build_command
        result.append({
            "directory": str(get_root_dir()),
            "arguments": build_command + ["-o", str(source_file.to_object_filename())],
            "file": str(source_file),
        })

    with open("compile_commands.json", "w") as f:
        f.write(json.dumps(result))


################################################################
########################## ENTRY POINT #########################
################################################################
def entry_point() -> None:
    program_name, *argv = sys.argv

    need_to_generate_build_commands: bool = False

    do_build: bool = True

    if consume_arg(argv, ["-h", "--help"]):
        usage(program_name, sys.stdout)
        sys.exit(0)
    if consume_arg(argv, "--verbose"):
        Logger.verbose = True
    if consume_arg(argv, "--clangd"):
        need_to_generate_build_commands = True
    if consume_arg(argv, "--force"):
        # dynamic languages have their positives :D
        SourceFile.was_modified = lambda *args, **kwargs: True
    if consume_arg(argv, "--clean"):
        define_details()

        Logger.info(f"cleaning project")

        Logger.info(f"removing {Executable.get().name}")
        Path(Executable.get().name).unlink(missing_ok=True)

        Logger.info(f"removing build directory")
        shutil.rmtree(str(get_build_directory()))

        Logger.info(f"removing compile_commands.json")
        (get_root_dir() / "compile_commands.json").unlink(missing_ok=True)

        Logger.info(f"removing pre-compiled headers")
        for precompiled_header in PreCompiledHeader.get():
            Path(str(precompiled_header.source.path) + ".gch").unlink(missing_ok=True)


        static_libraries_filepaths = set(str(static_library.out_filepath) for static_library in StaticLibrary.get())
        for filepath in static_libraries_filepaths:
            Logger.info(f"removing {filepath}")
            shutil.rmtree(filepath, ignore_errors=True)

        Logger.info("Done")
        sys.exit(0)
    if consume_arg(argv, "--full"):
        do_build = True
        need_to_generate_build_commands = True
    for arg in argv:
        if arg.startswith("-j"):
            if arg[2:] == "":
                cpu_count = os.cpu_count()
                CommandQueue.size = int(cpu_count * .75) if cpu_count is not None else 1
            else:
                try:
                    number = int(arg[2:])
                    if number <= 0:
                        raise ValueError
                    CommandQueue.size = number
                except Exception:
                    usage(program_name, sys.stderr)
                    Logger.error(f"`{arg[2:]}` is not a positive integer")
        else:
            Logger.error(f"unrecognized parater `{arg}`")

    define_details()

    if need_to_generate_build_commands:
        generate_clangd_commands()

    if do_build:
        PreCompiledHeader.build_all()
        rebuilt_static_libraries = StaticLibrary.build_all()
        rebuilt_executable = Executable.build()
        if rebuilt_static_libraries or rebuilt_executable or not Path(Executable.get().name).exists():
            Executable.link()
            Logger.info(f"Done")
        else:
            Logger.info(f"`{Executable.get().name}` already up to date")


################################################################
########################## "USER API" ##########################
################################################################
def get_root_dir() -> Path:
    return Path.cwd()


def define_details() -> None:
    # set_root(".")

    Compiler.set_toolchain("GNU")
    Compiler.set_c_standard(17)
    Compiler.set_cxx_standard(17)
    Compiler.add_build_flags("-g3", "-Og")
    # Compiler.add_link_flags("-Wl,--verbose")

    Compiler.add_define("DEBUG")
    Compiler.add_define("_DEBUG", "1")
    Compiler.add_define("CR_DEBUG")

    PreCompiledHeader(
        source="./Car/include/Car/Core/crpch.hpp",
        extra_build_flags=[],
        extra_defines=[]
    )
    
    StaticLibrary(
        name="glfw",
        out_filepath="./libraries/",
        sources=[
            "./vendor/glfw/src/context.c",
            "./vendor/glfw/src/init.c",
            "./vendor/glfw/src/input.c",
            "./vendor/glfw/src/monitor.c",
            "./vendor/glfw/src/null_init.c",
            "./vendor/glfw/src/null_joystick.c",
            "./vendor/glfw/src/null_monitor.c",
            "./vendor/glfw/src/null_window.c",
            "./vendor/glfw/src/platform.c",
            "./vendor/glfw/src/vulkan.c",
            "./vendor/glfw/src/window.c",
			"./vendor/glfw/src/x11_init.c",
			"./vendor/glfw/src/x11_monitor.c",
			"./vendor/glfw/src/x11_window.c",
			"./vendor/glfw/src/xkb_unicode.c",
			"./vendor/glfw/src/posix_module.c",
			"./vendor/glfw/src/posix_time.c",
			"./vendor/glfw/src/posix_thread.c",
			"./vendor/glfw/src/posix_module.c",
			"./vendor/glfw/src/posix_poll.c",
			"./vendor/glfw/src/glx_context.c",
			"./vendor/glfw/src/egl_context.c",
			"./vendor/glfw/src/osmesa_context.c",
			"./vendor/glfw/src/linux_joystick.c"
        ],
        extra_defines=[
            ("_GLFW_X11",),
        ],
        include_directories=[
            "./vendor/glfw/include/"
        ],
        depends_on=[]
    )
    StaticLibrary(
        name="ImGui",
        out_filepath="./libraries/",
            sources=[
            "./vendor/imgui/imgui_demo.cpp",
            "./vendor/imgui/imgui_draw.cpp",
            "./Car/src/ext/ImGui/imgui_impl_car.cpp",
            "./vendor/imgui/backends/imgui_impl_glfw.cpp",
            "./vendor/imgui/backends/imgui_impl_opengl3.cpp",
            "./vendor/imgui/imgui_tables.cpp",
            "./vendor/imgui/imgui_widgets.cpp",
            "./vendor/imgui/imgui.cpp",
        ],
        extra_defines=[
            ("GLFW_INCLUDE_NONE",),
        ],
        include_directories=[
            "./Car/include/",
            "./vendor/imgui/",
            "./vendor/glad/include/",
            "./vendor/glfw/include/",
            "./vendor/debugbreak/",
        ],
        depends_on=[
            "glad"
        ]
    )
    StaticLibrary(
        name="glad",
        out_filepath="./libraries/",
        sources=[
            "./vendor/glad/src/gl.c",
            "./vendor/glad/src/vulkan.c",
        ],
        include_directories=[
            "./vendor/glad/include/",
        ],
    )
    StaticLibrary(
        name="stb",
        out_filepath="./libraries/",
        sources=[
            "./vendor/stb/src/stb_image.c"
        ],
        include_directories=[
            "./vendor/stb/include/",
        ],
    )

    StaticLibrary(
        name="Car",
        out_filepath="./libraries/",
        sources=[
            "./Car/src/EntryPoint.cpp",
            "./Car/src/Utils.cpp",
            "./Car/src/Application.cpp",
            "./Car/src/Layers/Layer.cpp",
            "./Car/src/Layers/ImGuiLayer.cpp",
            "./Car/src/Layers/LayerStack.cpp",
            "./Car/src/Renderer/Buffer.cpp",
            "./Car/src/Renderer/Shader.cpp",
            "./Car/src/internal/GLFW/Window.cpp",
            "./Car/src/internal/GLFW/Input.cpp",
            "./Car/src/internal/GLFW/Time.cpp",
            "./Car/src/internal/OpenGL/GraphicsContext.cpp",
            "./Car/src/internal/OpenGL/Shader.cpp",
            "./Car/src/internal/OpenGL/IndexBuffer.cpp",
            "./Car/src/internal/OpenGL/VertexBuffer.cpp",
            "./Car/src/internal/OpenGL/VertexArray.cpp",
            "./Car/src/internal/OpenGL/UniformBuffer.cpp",
            "./Car/src/internal/OpenGL/Texture2D.cpp",
            "./Car/src/internal/OpenGL/Renderer.cpp",
        ],
        extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
        extra_defines=[
            ("CR_OPENGL",),
            ("GLFW_INCLUDE_NONE",),
        ],
        depends_on=[
            "ImGui", "glad", "stb", "glfw"
        ],
        include_directories=[
            "./Car/include/",
            "./vendor/stb/include/",
            "./vendor/glad/include/",
            "./vendor/glfw/include/",
            "./vendor/imgui/",
            "./vendor/glm/",
            "./vendor/debugbreak/",
        ],
    )

    Executable(
        name="sandbox.out",
        sources=[
            "SandBox/src/main.cpp",
        ],
        extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
        extra_link_flags=[],
        include_directories=[
            "./Car/include/",
            "./vendor/stb/include/",
            "./vendor/glad/include/",
            "./vendor/glfw/include/",
            "./vendor/imgui/",
            "./vendor/glm/",
            "./vendor/debugbreak/",
        ],
        libraries=[
            "GL",
            "fmt",
        ],
        extra_defines=[
            ("GLFW_INCLUDE_NONE",),
        ]
    )


if __name__ == "__main__":
    entry_point()
