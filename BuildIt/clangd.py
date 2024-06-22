from pathlib import Path

from BuildIt.register import Register
from BuildIt.compiler import Compiler
from BuildIt.logger import Logger

import json


def generate_clangd_commands() -> None:
    result = []

    for static_library in Register().static_libraries:
        c_build_command, cxx_build_command = Compiler.construct_build_command(static_library)

        for source_file in static_library.sources:
            Logger.info(f"generating command for {source_file}")
            if source_file.has_suffx(".cpp") or source_file.has_suffx(".cc"):
                build_command = cxx_build_command
            elif source_file.has_suffx(".c"):
                build_command = c_build_command
            else:
                Logger.warn(f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                build_command = cxx_build_command
            result.append({
                "directory": str(Path.cwd()),
                "arguments": build_command + ["-o", str(source_file.to_object_filename())],
                "file": str(source_file),
            })

    for executable in Register().executables:
        c_build_command, cxx_build_command = Compiler.construct_build_command(executable)

        for source_file in executable.sources:
            if source_file.has_suffx(".cpp") or source_file.has_suffx(".cc"):
                build_command = cxx_build_command
            elif source_file.has_suffx(".c"):
                build_command = c_build_command
            else:
                Logger.warn(f"unrecognized file extension `{source_file.path.suffix}` for {source_file}, assuming C++")
                build_command = cxx_build_command
            result.append({
                "directory": str(Path.cwd()),
                "arguments": build_command + ["-o", str(source_file.to_object_filename())],
                "file": str(source_file),
        })

    with open("compile_commands.json", "w") as f:
        f.write(json.dumps(result))