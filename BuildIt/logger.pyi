from typing import NoReturn, ClassVar


class Logger:
    verbose: ClassVar[bool] = False

    @classmethod
    def info(cls, message: str) -> None: ...
    @classmethod
    def warn(cls, message: str) -> None: ...
    @classmethod
    def error(cls, message: str, code: int=1) -> NoReturn: ...
