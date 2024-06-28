from .source_file import SourceFile


class PreCompiledHeader:
    source: SourceFile
    extra_build_flags: list[str] 
    extra_defines: dict[str, str | None]

    def __init__(
        self,
        source: str | SourceFile,
        extra_build_flags: list[str],
        extra_defines: list[tuple[str, str] | tuple[str]]
    ) -> None: ...
