from typing import Callable, Any

from BuildIt.source_file import SourceFile

from .compiler import Compiler, Toolchain
from .static_library import StaticLibrary
from .register import Register
from .command_queue import Command, CommandQueue, create_execute_command, create_gather_cmd
from .logger import Logger
from .log_file import LogFile
from .source_file import SourceFile
from .buildspec import BuildSpecFlags

from pathlib import Path
from collections import defaultdict, deque

import json
import time
import sys


_GNU = 1
_CLANG = 2


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


def load_include_structure() -> dict[str, list[str]]:
    inc_structure_file = Compiler.build_directory / "include_structure.json"
    inc_structure_file.parent.mkdir(parents=True, exist_ok=True)
    inc_structure_file.touch(exist_ok=True)

    with open(inc_structure_file, "r") as f:
        content = f.read()
        if content == "":
            return {}
        return json.loads(content)


def save_include_structure(data: dict[str, list[str]]) -> None:
    inc_structure_file = Compiler.build_directory / "include_structure.json"
    inc_structure_file.parent.mkdir(parents=True, exist_ok=True)
    inc_structure_file.touch(exist_ok=True)

    with open(inc_structure_file, "w") as f:
        f.truncate(0)
        f.write(json.dumps(data))


def load_compile_commands() -> dict[str, list[str]]:
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


def sort_static_libraries(_libraries: list[StaticLibrary]) -> list[StaticLibrary]:
    libraries: list[str] = list(lib.name for lib in _libraries)
    dependencies: dict[str, list[str]] = {lib.name: lib.depends_on for lib in _libraries}

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


def expand_static_libraries_from_strings(libraries: list[str]) -> list[StaticLibrary]:
    ret: set[StaticLibrary] = set()

    all_libraries = Register().static_libraries

    if not all_libraries:
        Logger.error(f"no static libraries detected to link against")

    start = time.time()
    while libraries:
        strlib = libraries.pop()

        for tlib in all_libraries:
            if tlib.name == strlib:
                ret.add(tlib)
                break
        else:
            Logger.error(f"static library {strlib} not found")

        # should never take 2 seconds
        if time.time() - start > 2:
            Logger.error("cyclical dependency detected in static libraries")

        if tlib.depends_on:
            libraries.extend(tlib.depends_on)

    return list(ret)


# path: (result of function)
_completed_paths: dict[Path, bool] = {}
_include_structure: dict[str, list[str]] = load_include_structure()
def find_if_file_changed_from_include_recursivly(target_file: Path | str, extra_include_directories: list[str], is_gathering: bool=False) -> bool:
    cached_value = _completed_paths.get(Path(target_file), None)
    if cached_value is not None:
        return cached_value
    
     
    includes: list[str] = []

    if not SourceFile(target_file).was_modified() and _include_structure.get(str(target_file), None) is not None:
        includes = _include_structure[str(target_file)]
    else:
        with open(target_file, "r") as f:
            for line in f.readlines():
                line = line.strip()
                # easy comment
                if line.startswith("//"):
                    continue
                if not "include" in line:
                    continue
                if not line.startswith("#"):
                    continue
                line = line[1:].strip()
                
                if line.startswith("include"):
                    include = line[8:].strip()[1:-1]
                    
                    paths_to_search = Compiler.include_directories + extra_include_directories
                    paths_to_search.insert(0, str(Path(target_file).parent))
                    if include.startswith("./"):
                        include = include[2:]
                        paths_to_search = [str(Path(target_file).parent)]
                    while include.startswith("../"):
                        include = include[3:]
                        paths_to_search = [str(Path(target_file).parent.parent)]
                    for include_dir in paths_to_search:
                        cpath = Path(include_dir) / include
                        if cpath.exists() and cpath.is_file():
                            includes.append(str(cpath))
        
        _include_structure[str(target_file)] = includes

    if len(includes) == 0:
        _completed_paths[Path(target_file)] = False
        return False

    for include in includes:
        if Path(target_file) not in _completed_paths:
            _completed_paths[Path(target_file)] = False
        if SourceFile(include).was_modified():
            _completed_paths[Path(target_file)] = True
            if is_gathering:
                LogFile.update(SourceFile(include))
            else:
                return True


    for include in includes:
        # some company made a header file that includes itself *cough cough google*
        # thus this line was introduced
        if include == str(target_file):
            continue
        if find_if_file_changed_from_include_recursivly(include, extra_include_directories, is_gathering):
            _completed_paths[Path(target_file)] = True
            if is_gathering:
                LogFile.update(include)
            else:
                return True

        _completed_paths[Path(target_file)] = False
    return False


def need_to_rebuild_source_file(source_file: SourceFile, compile_commands: dict[str, list[str]], compile_command: list[str], extra_include_directories: list[str]=[]) -> bool:
    aligns_with_compile_commands = compile_commands.get(str(source_file.path), None) is not None and compile_commands[str(source_file)] == compile_command
    includes_changed = find_if_file_changed_from_include_recursivly(str(source_file), extra_include_directories, False)
    return (not aligns_with_compile_commands) or includes_changed


def build_shared(toolc: int) -> None:
    sys.setrecursionlimit(100_000)

    compile_commands = load_compile_commands()

    changed_precompiled_headers: bool = False
    changed_static_libraries: bool = False
    changed_executables: bool = False

    all_static_libraries = Register().static_libraries

    link_commands: list[Command] = []

    if any(len(lib.attached_precompiled_headers) > 0 for lib in all_static_libraries):
        for static_library in all_static_libraries:
            c_build_command, cxx_build_command = Compiler.construct_build_command(static_library, True)

            for header_file in static_library.attached_precompiled_headers:
                if static_library.is_forced_cxx and toolc == _CLANG:
                    build_command = cxx_build_command + ["-x", "c++-header"]
                elif header_file.has_suffix(".hpp"):
                    build_command = cxx_build_command
                elif header_file.has_suffix(".h"):
                    build_command = c_build_command
                else:
                    Logger.warn(
                        f"unrecognized file extension `{header_file.path.suffix}` for {header_file}, assuming C++")
                    build_command = cxx_build_command


                if toolc == _CLANG:
                    pch_true_name = str(header_file) + ".pch"
                elif toolc == _GNU:
                    pch_true_name = str(header_file) + ".gch"
                else:
                    raise RuntimeError(toolc)

                if toolc == _CLANG:
                    command = build_command + ["-Xclang", "-emit-pch", str(header_file), "-o", pch_true_name]
                elif toolc == _GNU:
                    command = build_command + [str(header_file), "-o", pch_true_name]
                else:
                    raise RuntimeError(toolc)

                if header_file.was_modified() or not Path(pch_true_name).exists() or need_to_rebuild_source_file(header_file, compile_commands, command, []):
                    create_execute_command(
                        command,
                        f"compiling {header_file}",
                        f"failed to compile {header_file}"
                    )
                    changed_precompiled_headers = True
                    LogFile.update(header_file)
                    compile_commands[str(header_file.path)] = command

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
                if not source_file.object_file_upto_date() or need_to_rebuild_source_file(source_file, compile_commands, command, library.include_directories):
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

                object_files_in_static_library.append(object_file)

            if needs_rebuilding or not (library.out_filepath / f"lib{library.name}.a").exists():
                library.out_filepath.mkdir(parents=True, exist_ok=True)
                link_commands.append(create_execute_command(
                    ["ar", "rcs", "-o", str(library.out_filepath / f"lib{library.name}.a")] + list(
                        str(path) for path in object_files_in_static_library),
                    f"linking static library {library.out_filepath / f"lib{library.name}.a"}",
                    f"failed to create static library {library.out_filepath / f"lib{library.name}.a"}",
                    submit=False
                ))

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
                if not source_file.object_file_upto_date() or need_to_rebuild_source_file(source_file, compile_commands, command, executable.include_directories):
                    create_execute_command(
                        command,
                        f"compiling {source_file}",
                        f"failed to compile {source_file}"
                    )

                    LogFile.update(source_file)
                    changed_executables = True
                    compile_commands[str(source_file.path)] = command

        create_gather_cmd()

        for link_command in link_commands:
            CommandQueue.add(link_command)

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

            for static_library in sort_static_libraries(expand_static_libraries_from_strings(executable.static_libraries))[::-1]:
                command.append("-l")
                command.append(f"{str(static_library.name)}")

            if not Path(executable.name).exists() or changed_executables or changed_static_libraries:
                create_execute_command(
                    command,
                    f"creating executable `{executable.name}`",
                    "linker failed"
                )

        create_gather_cmd()
        
    save_include_structure(_include_structure)
    if not CommandQueue.holds_any_execute_commands():
        Logger.info("everything is up to date")
        return

    allow_seperate_thread = CommandQueue.size > 1

    if allow_seperate_thread:
        import _thread
        _lock = _thread.allocate_lock()

    def seperate_thread_func():
        if allow_seperate_thread:
            CommandQueue.size -= 1
            _lock.acquire()
        _completed_paths.clear()

        if any(len(lib.attached_precompiled_headers) > 0 for lib in all_static_libraries):
            for static_library in all_static_libraries:
                for header_file in static_library.attached_precompiled_headers:
                    find_if_file_changed_from_include_recursivly(header_file.path, static_library.include_directories, is_gathering=True)

        if len(all_static_libraries) > 0:
            for library in all_static_libraries:
                for source_file in library.sources:
                    find_if_file_changed_from_include_recursivly(source_file.path, library.include_directories, is_gathering=True)

        if len(Register().executables) > 0:
            for executable in Register().executables:
                for source_file in executable.sources:
                    find_if_file_changed_from_include_recursivly(source_file.path, executable.include_directories, is_gathering=True)

        if allow_seperate_thread:
            CommandQueue.size += 1
            _lock.release()

    if allow_seperate_thread:
        _thread.start_new_thread(seperate_thread_func, ())
    else:
        seperate_thread_func()

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

    if allow_seperate_thread:
        # make sure that the function completes before we exit the scope
        _lock.acquire()
        _lock.release()

    LogFile.save()


def build() -> None:
    Functions.execute()
    try:
        if Compiler.toolchain == Toolchain.GNU:
            return build_shared(_GNU)
        elif Compiler.toolchain == Toolchain.CLANG:
            return build_shared(_CLANG)
        else:
            Logger.error(f"unrecognized toolchain `{Compiler.toolchain}`, build.py")
    except KeyboardInterrupt:
        Logger.info("Aborting")
