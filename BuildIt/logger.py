from typing import NoReturn

import sys


class Logger:
    verbose = False

    @classmethod
    def info(cls, message):
        if not cls.verbose:
            return
        print(f"[INFO] {message}", file=sys.stdout)

    @classmethod
    def warn(cls, message):
        print(f"[WARNING] {message}", file=sys.stderr)

    @classmethod
    def error(cls, message, code=1):
        print(f"\033[31m[ERROR] {message}\033[0m", file=sys.stderr)
        sys.exit(code)
