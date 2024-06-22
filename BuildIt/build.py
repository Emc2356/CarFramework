from BuildIt.compiler import Compiler
from BuildIt.source_file import SourceFile
from BuildIt.static_library import StaticLibrary
from BuildIt.precompiled_header import PreCompiledHeader
from BuildIt.executable import Executable
from BuildIt.register import Register
from BuildIt.command_queue import CommandQueue
from BuildIt.logger import Logger
from BuildIt.log_file import LogFile

from pathlib import Path
from collections import defaultdict, deque


def sort_static_libraries() -> list[StaticLibrary]:
    libraries: list[str] = list(lib.name for lib in Register().static_libraries)
    dependencies: dict[str, list[str]] = {lib.name: lib.depends_on for lib in Register().static_libraries}

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

    if len(Register().precompiled_headers) > 0:
        for header in Register().precompiled_headers:
            c_build_command, cxx_build_command = Compiler.construct_build_command(header)

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
                changed_precompiled_headers = True

                LogFile.update(header.source)
            else:
                Logger.info(f"`{header.source}` already up to date")
    if len(Register().static_libraries) > 0:
        for library in Register().static_libraries:
            c_build_command, cxx_build_command = Compiler.construct_build_command(library)

            object_files_in_static_library: list[Path] = []
            needs_rebuilding: bool = False
            for source_file in library.sources:
                if source_file.has_suffx(".cpp") or source_file.has_suffx(".cc"):
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
                if not source_file.object_file_upto_date() or changed_precompiled_headers:
                    CommandQueue.add(
                        build_command + ["-o", str(object_file), str(source_file)],
                        f"compiling {source_file}",
                        f"failed to compile {source_file}"
                    )

                    # save the time
                    LogFile.update(str(source_file))
                    changed_static_libraries = True
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

        CommandQueue.flush()
    if len(Register().executables) > 0:
        for executable in Register().executables:
            c_build_command, cxx_build_command = Compiler.construct_build_command(executable)

            for source_file in executable.sources:
                Logger.info(f"{source_file}")

                Logger.info(f"generating command for {source_file}")
                if source_file.has_suffx(".cpp") or source_file.has_suffx(".cc"):
                    build_command = cxx_build_command
                elif source_file.has_suffx(".c"):
                    build_command = c_build_command
                else:
                    Logger.warn(f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                    build_command = cxx_build_command

                object_file = source_file.to_object_filename()

                object_file.parent.mkdir(parents=True, exist_ok=True)

                if not source_file.object_file_upto_date() or changed_static_libraries:
                    CommandQueue.add(
                        build_command + ["-o", str(object_file), str(source_file)],
                        f"compiling {source_file}",
                        f"failed to compile {source_file}"
                    )

                    LogFile.update(source_file)
                    changed_executables = True
                else:
                    Logger.info(f"`{source_file}` already up to date")

            CommandQueue.flush()

        for executable in Register().executables:
            command: list[str] = [Compiler.linker] + Compiler.link_flags + executable.extra_link_flags

            static_library_directories: set = set()
            for static_library in Register().static_libraries:
                static_library_directories.add(static_library.out_filepath)

            for dir in static_library_directories:
                command.append("-L")
                command.append(str(dir))

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

            if not Path(executable.name).exists() or changed_executables:
                CommandQueue.add(
                    command,
                    f"linking objects into `{executable.name}`",
                    "linker failed"
                )

        CommandQueue.flush()

def build() -> None:
    if Compiler.toolchain == "gnu":
        return build_gnu()
    else:
        Logger.error(f"unrecognized toolchain `{Compiler.toolchain}`, build.py")
