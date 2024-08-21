from typing import TextIO, Optional, Callable

from pathlib import Path
from inspect import signature
import shutil

from .logger import Logger
from .command_queue import CommandQueue
from .compiler import Compiler
from .source_file import SourceFile
from .register import Register
from .clangd import generate_clangd_commands

import sys
import os


def usage(out: TextIO) -> None:
    print("BuildIt CLI Usage:", file=out)
    print("    -h, --help display this message", file=out)
    print(
        "    -j<number> specify the amount of commands it can run simultaneously, if no number is then it will be set to use 75% of your cores",
        file=out)
    print("    --release uses optimization flags for the compiler that will be used", file=out)
    print("    --clangd generates the compile_commands.json file for clangd lsp", file=out)
    print("    --force force rebuilds everything", file=out)
    print("    --clean cleans everything produced by this script", file=out)


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


class CLIData:
    unknown_argument_callback: Optional[Callable[[str], bool] | Callable[[str, list[str]], bool]] = None


def unknown_argument(func):
    CLIData.unknown_argument_callback = func

    return func


def handle_argv(argv = None):
    if argv is None:
        argv = sys.argv[1:]
    if consume_arg(argv, ["-h", "--help"]):
        usage(sys.stdout)
        if CLIData.unknown_argument_callback is not None:
            if len(signature(CLIData.unknown_argument_callback).parameters) == 1:
                CLIData.unknown_argument_callback("-h")  # type: ignore[call-arg]
            elif len(signature(CLIData.unknown_argument_callback).parameters) == 2:
                CLIData.unknown_argument_callback(arg, argv[i + 1:])  # type: ignore[call-arg]
            else:
                Logger.error(f"unsupported amount of parameters for the unknown_argument callback")
        sys.exit(0)
    if consume_arg(argv, "--clangd"):
        generate_clangd_commands()
        sys.exit(0)
    if consume_arg(argv, "--force"):
        # dynamic languages have their positives :D
        # probably bad way to do this, but it works :/
        SourceFile.was_modified = lambda *args, **kwargs: True  # type: ignore[method-assign]
    if consume_arg(argv, "--release"):
        Compiler.is_release = True
    if consume_arg(argv, "--clean"):
        from .build import Functions
        Functions.execute()

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
            # TODO: this is clang specific
            Path(str(precompiled_header.source.path) + ".pch").unlink(missing_ok=True)

        for filepath in set(
                str(static_library.out_filepath)
                for static_library in Register().static_libraries
        ):
            Logger.info(f"removing {filepath}")
            shutil.rmtree(filepath, ignore_errors=True)

        Logger.info("Done")
        sys.exit(0)

    i: int = 0
    while i < len(argv):
        arg = argv[i]
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
                    # CommandQueue.size = number
                    CommandQueue.size = number
                except ValueError:
                    usage(sys.stderr)
                    Logger.error(f"`{arg[2:]}` is not a positive integer")
        else:
            args_to_skip: int = 0
            if CLIData.unknown_argument_callback is not None:
                if len(signature(CLIData.unknown_argument_callback).parameters) == 1:
                    if CLIData.unknown_argument_callback(arg):  # type: ignore[call-arg]
                        i += 1
                        continue
                elif len(signature(CLIData.unknown_argument_callback).parameters) == 2:
                    args_to_skip = CLIData.unknown_argument_callback(arg, argv[i + 1:])  # type: ignore[call-arg]
                    if args_to_skip > 0:
                        i += args_to_skip
                        continue
                else:
                    Logger.error(f"unsupported amount of parameters for the unknown_argument callback")
                if args_to_skip > 0:
                    continue
            Logger.error(f"unrecognized parameter `{arg}`")

        i += 1
