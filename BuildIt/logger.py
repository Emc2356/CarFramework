from typing import NoReturn

import sys


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
    def error(cls, message: str, code: int=1) -> NoReturn:
        print(f"\033[31m[ERROR] {message}\033[0m", file=sys.stderr)
        sys.exit(code)
