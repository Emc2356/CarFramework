from BuildIt.logger import Logger
from BuildIt.command_queue import CommandQueue
from BuildIt.source_file import SourceFile


class PreCompiledHeader:
    __slots__ = ("source", "extra_build_flags", "extra_defines")

    def __init__(self, source: str | SourceFile, extra_build_flags: list[str], extra_defines: list[tuple[str, str] | tuple[str]]) -> None:
        self.source: SourceFile = SourceFile.from_list([source])[0]
        self.extra_build_flags: list[str] = extra_build_flags
        self.extra_defines: dict[str, str | None] = {define[0]: (define[1] if len(define) == 2 else None) for define in extra_defines}

        # shitty ass circular imports :(
        from BuildIt.register import Register
        
        Register.submit(self)
