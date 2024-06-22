from typing import TextIO

from pathlib import Path
import shutil

from BuildIt.logger import Logger
from BuildIt.command_queue import CommandQueue
from BuildIt.source_file import SourceFile
from BuildIt.register import Register
from BuildIt.clangd import generate_clangd_commands

import sys
import os 


def usage(out: TextIO) -> None:
    print("BuildIt CLI Usage:", file=out)
    print("    -h, --help display this message", file=out)
    print("    --verbose show info messages", file=out)
    print("    -j<number> specify the amount of commands it can run simultaneously, if no number is then it will be set to use 75% of your cores", file=out)
    print("    --clangd generates the compile_commands.json file for clangd extension", file=out)
    print("    --force force rebuilds everything", file=out)
    print("    --clean cleans everything produced by this script, default is false", file=out)


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
        Logger.error(f"BuildIt error: :consume_arg({argv}, {argument}):")
    return False

def handle_argv(argv: list[str]) -> None:
    if consume_arg(argv, ["-h", "--help"]):
        usage(sys.stdout)
        sys.exit(0)
    if consume_arg(argv, "--verbose"):
        Logger.verbose = True
    if consume_arg(argv, "--clangd"):
        generate_clangd_commands()
    if consume_arg(argv, "--force"):
        # dynamic languages have their positives :D
        # propably bad way to do this but it works :/
        SourceFile.was_modified = lambda *args, **kwargs: True # type: ignore[method-assign]
    if consume_arg(argv, "--clean"):
        Logger.info(f"cleaning project")

        for executable in Register().executables:
            Logger.info(f"removing {executable.name}")
            Path(executable.name).unlink(missing_ok=True)

        Logger.info(f"removing build directory")
        try:
            shutil.rmtree("./build")
        except FileNotFoundError:
            pass

        Logger.info(f"removing compile_commands.json")
        Path("./compile_commands.json").unlink(missing_ok=True)

        Logger.info(f"removing pre-compiled headers")
        for precompiled_header in Register().precompiled_headers:
            # TODO: this is gnu specific
            Path(str(precompiled_header.source.path) + ".gch").unlink(missing_ok=True)


        static_libraries_filepaths = set(str(static_library.out_filepath) for static_library in Register().static_libraries)
        for filepath in static_libraries_filepaths:
            Logger.info(f"removing {filepath}")
            shutil.rmtree(filepath, ignore_errors=True)

        Logger.info("Done")
        sys.exit(0)
    
    for arg in argv:
        if arg.startswith("-j"):
            if arg[2:] == "":
                # apparently sometimes cpu_count returns None, who knew
                cpu_count = os.cpu_count()
                CommandQueue.size = int(cpu_count * .75) if cpu_count is not None else 1
            else:
                try:
                    number = int(arg[2:])
                    if number <= 0:
                        raise ValueError
                    CommandQueue.size = number
                except Exception:
                    usage(sys.stderr)
                    Logger.error(f"`{arg[2:]}` is not a positive integer")
        else:
            Logger.error(f"unrecognized parater `{arg}`")