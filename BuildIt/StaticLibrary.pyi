from pathlib import Path
from .source_file import SourceFile
from .register import Register


class StaticLibrary:
    name: str
    out_filepath: Path
    sources: list[SourceFile]
    extra_build_flags: list[str]
    extra_defines: list[tuple[str, str] | tuple[str]]
    depends_on: list[str]
    include_directories: list[str]
    
    def __init__(
        self,
        name: str,
        out_filepath: str,
        sources: list[str | SourceFile],
        extra_build_flags: list[str]=[],
        extra_defines: list[tuple[str, str] | tuple[str]]=[],
        depends_on: list[str]=[],
        include_directories: list[str]=[]
    ): ...
