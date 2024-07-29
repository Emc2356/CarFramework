from typing import Callable, Any

from BuildIt.precompiled_header import PreCompiledHeader
from BuildIt.source_file import SourceFile

from .compiler import Compiler, Toolchain
from .static_library import StaticLibrary
from .register import Register
from .command_queue import CommandQueue, create_execute_command, create_gather_cmd
from .logger import Logger
from .log_file import LogFile
from .source_file import SourceFile
from .build_spec_flags import BuildSpecFlags

from pathlib import Path
from collections import defaultdict, deque

import json
import time
import sys


class Functions:
    _instance = None
    _executed = False

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(Functions, cls).__new__(cls, *args, **kwargs)
            cls._instance.__init()
        return cls._instance

    def __init(self) -> None:
        self.functions: list[tuple[BuildSpecFlags, Callable[[], Any]]] = []

    @staticmethod
    def add_function(flags: BuildSpecFlags, func) -> None:
        Functions().functions.append((flags, func))

    @staticmethod
    def execute() -> None:
        if Functions._executed:
            return
        Functions._executed = True
        
        instance = Functions()
        core_funcs: list[Callable[[], Any]] = []

        for flags, func in instance.functions:
            if not (flags & BuildSpecFlags.CORE):
                continue
            core_funcs.append(func)

        if len(core_funcs) == 0:
            Logger.error("no core function declaration found")
        elif len(core_funcs) > 1:
            Logger.error(f"too many core function declaration found ({len(core_funcs)})")

        core_funcs[0]()

        is_gnu: bool = Compiler.toolchain == Toolchain.GNU
        is_clang: bool = Compiler.toolchain == Toolchain.CLANG
        is_msvc: bool = Compiler.toolchain == Toolchain.MSVC

        for flags, func in instance.functions:
            # core functions have been handled already
            if flags & BuildSpecFlags.CORE:
                continue

            if ((flags & BuildSpecFlags.GNU and is_gnu) or
                    (flags & BuildSpecFlags.CLANG and is_clang) or
                    (flags & BuildSpecFlags.MSVC and is_msvc)):
                func()


class LocalOptions:
    rebuild_if_includes_changed: bool = False


def rebuild_if_includes_changed() -> None:
    LocalOptions.rebuild_if_includes_changed = True


def find_if_file_changed_from_include_recursivly(path: Path | str, depth=150, is_gathering: bool=False) -> bool:
    if not LocalOptions.rebuild_if_includes_changed:
        return False
    if depth == 0:
        return False
    
    Functions.execute()
    includes: list[str] = []
    
    with open(path, "r") as f:
        in_comment = False
        for line in f.readlines():
            line = line.strip()
            # easy comment
            if line.startswith("//"):
                continue
            if "/*" in line:
                in_comment = True
            if "*/" in line:
                in_comment = False
            if in_comment or not "include" in line:
                continue
            if line.endswith("*/") or not line.startswith("#"):
                continue
            line = line[1:].strip()
            if line.startswith("include"):
                include = line[8:].strip()[1:-1]
                for include_dir in Compiler.include_directories:
                    if (Path(include_dir) / include).exists():
                        if SourceFile(Path(include_dir) / include).was_modified():
                            if is_gathering:
                                LogFile.update(SourceFile(Path(include_dir) / include))
                            if not is_gathering: 
                                return True
                        includes.append(str(Path(include_dir) / include))
                        break
                else:
                    continue
    
    if len(includes) == 0:
        return False
    
    for include in includes:
        if find_if_file_changed_from_include_recursivly(include, depth-1, is_gathering):
            if is_gathering:
                LogFile.update(include)
            if not is_gathering: 
                return True
    
    return False


def get_compile_commands() -> dict[str, list[str]]:
    commands_file = Compiler.build_directory / "compile_commands.json"
    commands_file.parent.mkdir(parents=True, exist_ok=True)
    commands_file.touch(exist_ok=True)
    
    with open(commands_file, "r") as f:
        content = f.read()
        if content == "":
            return {}
        return json.loads(content)
        
        
def save_compile_commands(data: dict) -> None:
    commands_file = Compiler.build_directory / "compile_commands.json"
    commands_file.touch(exist_ok=True)
    
    with open(commands_file, "w") as f:
        f.truncate(0)
        f.write(json.dumps(data))


def sort_static_libraries() -> list[StaticLibrary]:
    libraries: list[str] = list(lib.name for lib in Register().static_libraries)
    dependencies: dict[str, list[str]] = {lib.name: lib.depends_on for lib in Register().static_libraries}

    for name, deps in dependencies.items():
        for dep in deps:
            if dep not in dependencies.keys():
                Logger.error(
                    f"unknown static library `{dep}` provided for the depends_on option for static library `{name}`")

    # Create a graph
    graph = defaultdict(list)
    in_degree = {lib: 0 for lib in libraries}

    # Build the graph and in-degree count
    for lib, deps in dependencies.items():
        for dep in deps:
            graph[dep].append(lib)
            in_degree[lib] += 1

    # Find all nodes with no incoming edges
    queue = deque([lib for lib in libraries if in_degree[lib] == 0])

    sorted_static_libraries_names = []

    while queue:
        lib = queue.popleft()
        sorted_static_libraries_names.append(lib)

        # Decrease the in-degree of the neighboring nodes
        for neighbor in graph[lib]:
            in_degree[neighbor] -= 1
            if in_degree[neighbor] == 0:
                queue.append(neighbor)

    # Check if there was a cycle
    if len(sorted_static_libraries_names) != len(libraries):
        Logger.error("Cycle detected in dependencies")

    sorted_static_libraries: list[StaticLibrary] = []
    for name in sorted_static_libraries_names:
        for library in Register().static_libraries:
            if name == library.name:
                sorted_static_libraries.append(library)

    return sorted_static_libraries


def build_gnu() -> None:
    changed_precompiled_headers: bool = False
    changed_static_libraries: bool = False
    changed_executables: bool = False

    all_precompiled_headers = Register().precompiled_headers
    all_static_libraries = Register().static_libraries

    if len(all_precompiled_headers) > 0 or any(len(lib.attached_precompiled_headers) > 0 for lib in all_static_libraries):
        for header in all_precompiled_headers:
            c_build_command, cxx_build_command = Compiler.construct_build_command(header, True)

            if header.source.has_suffix(".hpp"):
                build_command = cxx_build_command
            elif header.source.has_suffix(".h"):
                build_command = c_build_command
            else:
                Logger.warn(
                    f"unrecognized file extension `{header.source.path.suffix}` for {header.source}, assuming C++")
                build_command = cxx_build_command

            if header.source.was_modified() or not Path(str(header.source) + ".gch").exists():
                create_execute_command(
                    build_command + [str(header.source), "-o", f"{str(header.source)}.gch"],
                    f"compiling {header.source}",
                    f"failed to compile {header.source}"
                )
                changed_precompiled_headers = True

                LogFile.update(header.source)
            else:
                Logger.info(f"{header.source} already up to date")

        for static_library in all_static_libraries:
            c_build_command, cxx_build_command = Compiler.construct_build_command(static_library, True)
            
            for header_file in static_library.attached_precompiled_headers:
                if static_library.is_forced_cxx:
                    build_command = cxx_build_command
                elif header_file.has_suffix(".hpp"):
                    build_command = cxx_build_command
                elif header_file.has_suffix(".h"):
                    build_command = c_build_command
                else:
                    Logger.warn(
                        f"unrecognized file extension `{header_file.path.suffix}` for {header_file}, assuming C++")
                    build_command = cxx_build_command

                if header_file.was_modified() or not Path(str(header_file) + ".gch").exists():
                    create_execute_command(
                        build_command + [str(header_file), "-o", f"{str(header_file)}.gch"],
                        f"compiling {header_file}",
                        f"failed to compile {header_file}"
                    )
                    changed_precompiled_headers = True
    
                    LogFile.update(header_file)
                else:
                    Logger.info(f"{header_file} already up to date")
        
        create_gather_cmd()
    
    if len(all_static_libraries) > 0:
        for library in all_static_libraries:
            c_build_command, cxx_build_command = Compiler.construct_build_command(library)

            object_files_in_static_library: list[Path] = []
            needs_rebuilding: bool = False
            for source_file in library.sources:
                if source_file.has_suffix(".cpp") or source_file.has_suffix(".cc"):
                    build_command = cxx_build_command
                elif source_file.has_suffix(".c"):
                    build_command = c_build_command
                else:
                    Logger.warn(
                        f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                    build_command = cxx_build_command

                object_file = source_file.to_object_filename()

                # ensure that the compiler doesn't fail because the parent directory doesn't exist
                object_file.parent.mkdir(parents=True, exist_ok=True)

                # check if the object file is up-to-date
                if not source_file.object_file_upto_date() or changed_precompiled_headers:
                    create_execute_command(
                        build_command + ["-o", str(object_file), str(source_file)],
                        f"compiling {source_file}",
                        f"failed to compile {source_file}"
                    )

                    # save the time
                    LogFile.update(str(source_file))
                    changed_static_libraries = True
                    needs_rebuilding = True
                else:
                    Logger.info(f"{source_file} already up to date")

                object_files_in_static_library.append(object_file)

            # make sure that the object files exist before the library is created
            create_gather_cmd()
            if needs_rebuilding or not (library.out_filepath / f"lib{library.name}.a").exists():
                library.out_filepath.mkdir(parents=True, exist_ok=True)
                create_execute_command(
                    ["ar", "rcs", "-o", str(library.out_filepath / f"lib{library.name}.a")] + list(
                        str(path) for path in object_files_in_static_library),
                    f"linking static library {library.out_filepath / f"lib{library.name}.a"}",
                    f"failed to create static library {library.out_filepath / f"lib{library.name}.a"}"
                )

    if len(Register().executables) > 0:
        for executable in Register().executables:
            c_build_command, cxx_build_command = Compiler.construct_build_command(executable)

            for source_file in executable.sources:
                if source_file.has_suffix(".cpp") or source_file.has_suffix(".cc"):
                    build_command = cxx_build_command
                elif source_file.has_suffix(".c"):
                    build_command = c_build_command
                else:
                    Logger.warn(
                        f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                    build_command = cxx_build_command

                object_file = source_file.to_object_filename()

                object_file.parent.mkdir(parents=True, exist_ok=True)

                if not source_file.object_file_upto_date():
                    create_execute_command(
                        build_command + ["-o", str(object_file), str(source_file)],
                        f"compiling {source_file}",
                        f"failed to compile {source_file}"
                    )

                    LogFile.update(source_file)
                    changed_executables = True
                else:
                    Logger.info(f"{source_file} already up to date")

        create_gather_cmd()

        for executable in Register().executables:
            command: list[str] = [Compiler.linker] + Compiler.link_flags + executable.extra_link_flags

            static_library_directories: set = set()
            for static_library in all_static_libraries:
                static_library_directories.add(static_library.out_filepath)

            for directory in static_library_directories:
                command.append("-L")
                command.append(str(directory))

            command.append("-o")
            command.append(executable.name)

            for source_file in executable.sources:
                command.append(str(source_file.to_object_filename()))

            for lib in executable.libraries:
                command.append("-l")
                command.append(f"{lib}")

            for static_library in sort_static_libraries()[::-1]:
                command.append("-l")
                command.append(f"{str(static_library.name)}")
            
            if not Path(executable.name).exists() or changed_executables or changed_static_libraries:
                create_execute_command(
                    command,
                    f"linking objects into `{executable.name}`",
                    "linker failed"
                )

        create_gather_cmd()
        
    if len(all_precompiled_headers) > 0 or any(len(lib.attached_precompiled_headers) > 0 for lib in all_static_libraries):
        for header in all_precompiled_headers:
            find_if_file_changed_from_include_recursivly(header.source.path, is_gathering=True)

        for static_library in all_static_libraries:
            for header_file in static_library.attached_precompiled_headers:
                find_if_file_changed_from_include_recursivly(header_file.path, is_gathering=True)
    
    if len(all_static_libraries) > 0:
        for library in all_static_libraries:
            for source_file in library.sources:
                find_if_file_changed_from_include_recursivly(source_file.path, is_gathering=True)

    if len(Register().executables) > 0:
        for executable in Register().executables:
            for source_file in executable.sources:
                find_if_file_changed_from_include_recursivly(source_file.path, is_gathering=True)
    
    start_time = time.time()
    CommandQueue.execute()
    total_time = time.time() - start_time
    minutes = int(total_time // 60)
    seconds = int(total_time - minutes * 60)
    if minutes > 0:
        Logger.info(f"compilation took {minutes}m {seconds}s")
    else:
        Logger.info(f"compilation took {seconds}s")


def build_clang() -> None:
    compile_commands = get_compile_commands()
    
    changed_precompiled_headers: bool = False
    changed_static_libraries: bool = False
    changed_executables: bool = False

    all_precompiled_headers = Register().precompiled_headers
    all_static_libraries = Register().static_libraries

    if len(all_precompiled_headers) > 0 or any(len(lib.attached_precompiled_headers) > 0 for lib in all_static_libraries):
        for header in all_precompiled_headers:
            c_build_command, cxx_build_command = Compiler.construct_build_command(header, True)

            if header.source.has_suffix(".hpp"):
                build_command = cxx_build_command
            elif header.source.has_suffix(".h"):
                build_command = c_build_command
            else:
                Logger.warn(
                    f"unrecognized file extension `{header.source.path.suffix}` for {header.source}, assuming C++")
                build_command = cxx_build_command

            command = build_command + ["-Xclang", "-emit-pch", str(header.source), "-o", f"{str(header.source)}.pch"]
            if header.source.was_modified() or not Path(str(header.source) + ".pch").exists() or (compile_commands.get(str(header.source), None) is None or compile_commands[str(header.source)] != command) or find_if_file_changed_from_include_recursivly(header.source.path):
                create_execute_command(
                    command,
                    f"compiling {header.source}",
                    f"failed to compile {header.source}"
                )
                changed_precompiled_headers = True
                LogFile.update(header.source)
                compile_commands[str(header.source)] = command
            else:
                Logger.info(f"{header.source} already up to date")

        for static_library in all_static_libraries:
            c_build_command, cxx_build_command = Compiler.construct_build_command(static_library, True)
            
            for header_file in static_library.attached_precompiled_headers:
                if static_library.is_forced_cxx:
                    build_command = cxx_build_command + ["-x", "c++-header"]
                elif header_file.has_suffix(".hpp"):
                    build_command = cxx_build_command
                elif header_file.has_suffix(".h"):
                    build_command = c_build_command
                else:
                    Logger.warn(
                        f"unrecognized file extension `{header_file.path.suffix}` for {header_file}, assuming C++")
                    build_command = cxx_build_command

                command = build_command + ["-Xclang", "-emit-pch", str(header_file), "-o", f"{str(header_file)}.pch"]
                if header_file.was_modified() or not Path(str(header_file) + ".pch").exists() or (compile_commands.get(str(header_file.path), None) is None or compile_commands[str(header_file.path)] != command) or find_if_file_changed_from_include_recursivly(header_file.path):
                    create_execute_command(
                        command,
                        f"compiling {header_file}",
                        f"failed to compile {header_file}"
                    )
                    changed_precompiled_headers = True
                    LogFile.update(header_file)
                    compile_commands[str(header_file.path)] = command
                else:
                    Logger.info(f"{header_file} already up to date")
        
        create_gather_cmd()
    
    if len(all_static_libraries) > 0:
        for library in all_static_libraries:
            c_build_command, cxx_build_command = Compiler.construct_build_command(library)

            object_files_in_static_library: list[Path] = []
            needs_rebuilding: bool = False
            for source_file in library.sources:
                if source_file.has_suffix(".cpp") or source_file.has_suffix(".cc"):
                    build_command = cxx_build_command
                elif source_file.has_suffix(".c"):
                    build_command = c_build_command
                else:
                    Logger.warn(
                        f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                    build_command = cxx_build_command

                object_file = source_file.to_object_filename()

                # ensure that the compiler doesn't fail because the parent directory doesn't exist
                object_file.parent.mkdir(parents=True, exist_ok=True)

                # check if the object file is up-to-date
                command = build_command + ["-o", str(object_file), str(source_file)]
                if not source_file.object_file_upto_date() or (compile_commands.get(str(source_file.path), None) is None or compile_commands[str(source_file.path)] != command) or find_if_file_changed_from_include_recursivly(source_file.path):
                    create_execute_command(
                        command,
                        f"compiling {source_file}",
                        f"failed to compile {source_file}"
                    )

                    # save the time
                    LogFile.update(str(source_file))
                    changed_static_libraries = True
                    needs_rebuilding = True
                    compile_commands[str(source_file.path)] = command
                else:
                    Logger.info(f"{source_file} already up to date")

                object_files_in_static_library.append(object_file)

            # make sure that the object files exist before the library is created
            create_gather_cmd()
            if needs_rebuilding or not (library.out_filepath / f"lib{library.name}.a").exists():
                library.out_filepath.mkdir(parents=True, exist_ok=True)
                create_execute_command(
                    ["ar", "rcs", "-o", str(library.out_filepath / f"lib{library.name}.a")] + list(
                        str(path) for path in object_files_in_static_library),
                    f"linking static library {library.out_filepath / f"lib{library.name}.a"}",
                    f"failed to create static library {library.out_filepath / f"lib{library.name}.a"}"
                )

    if len(Register().executables) > 0:
        for executable in Register().executables:
            c_build_command, cxx_build_command = Compiler.construct_build_command(executable)

            for source_file in executable.sources:
                if source_file.has_suffix(".cpp") or source_file.has_suffix(".cc"):
                    build_command = cxx_build_command
                elif source_file.has_suffix(".c"):
                    build_command = c_build_command
                else:
                    Logger.warn(
                        f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                    build_command = cxx_build_command

                object_file = source_file.to_object_filename()

                object_file.parent.mkdir(parents=True, exist_ok=True)

                command = build_command + ["-o", str(object_file), str(source_file)]
                if not source_file.object_file_upto_date() or (compile_commands.get(str(source_file.path), None) is None or compile_commands[str(source_file.path)] != command) or find_if_file_changed_from_include_recursivly(source_file.path):
                    create_execute_command(
                        command,
                        f"compiling {source_file}",
                        f"failed to compile {source_file}"
                    )

                    LogFile.update(source_file)
                    changed_executables = True
                    compile_commands[str(source_file.path)] = command
                else:
                    Logger.info(f"{source_file} already up to date")

        create_gather_cmd()

        for executable in Register().executables:
            command: list[str] = [Compiler.linker] + Compiler.link_flags + executable.extra_link_flags

            static_library_directories: set = set()
            for static_library in all_static_libraries:
                static_library_directories.add(static_library.out_filepath)

            for directory in static_library_directories:
                command.append("-L")
                command.append(str(directory))

            command.append("-o")
            command.append(executable.name)

            for source_file in executable.sources:
                command.append(str(source_file.to_object_filename()))

            for lib in executable.libraries:
                command.append("-l")
                command.append(f"{lib}")

            for static_library in sort_static_libraries()[::-1]:
                command.append("-l")
                command.append(f"{str(static_library.name)}")
            
            if not Path(executable.name).exists() or changed_executables or changed_static_libraries:
                create_execute_command(
                    command,
                    f"linking objects into `{executable.name}`",
                    "linker failed"
                )

        create_gather_cmd()
    
    if len(all_precompiled_headers) > 0 or any(len(lib.attached_precompiled_headers) > 0 for lib in all_static_libraries):
        for header in all_precompiled_headers:
            find_if_file_changed_from_include_recursivly(header.source.path, is_gathering=True)

        for static_library in all_static_libraries:
            for header_file in static_library.attached_precompiled_headers:
                find_if_file_changed_from_include_recursivly(header_file.path, is_gathering=True)
    
    if len(all_static_libraries) > 0:
        for library in all_static_libraries:
            for source_file in library.sources:
                find_if_file_changed_from_include_recursivly(source_file.path, is_gathering=True)

    if len(Register().executables) > 0:
        for executable in Register().executables:
            for source_file in executable.sources:
                find_if_file_changed_from_include_recursivly(source_file.path, is_gathering=True)
                    

    
    start_time = time.time()
    CommandQueue.execute()
    total_time = time.time() - start_time
    minutes = int(total_time // 60)
    seconds = int(total_time - minutes * 60)
    if minutes > 0:
        Logger.info(f"compilation took {minutes}m {seconds}s")
    else:
        Logger.info(f"compilation took {seconds}s")
        
    save_compile_commands(compile_commands)


def build() -> None:
    Functions.execute()
    if Compiler.toolchain == Toolchain.GNU:
        return build_gnu()
    elif Compiler.toolchain == Toolchain.CLANG:
        return build_clang()
    else:
        Logger.error(f"unrecognized toolchain `{Compiler.toolchain}`, build.py")
