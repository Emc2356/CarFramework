from typing import Self

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
    attached_precompiled_headers: list[SourceFile]
    is_forced_cxx: bool
    is_optimized: bool
    
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
    def add_sources(self, *sources) -> Self: ...
    def add_define(self, name: str, value: str | None=None) -> Self: ...
    def attach_precompiled_headers(self, *paths) -> Self: ...
    def force_language_cxx(self) -> Self: ...
    def always_optimize(self) -> Self: ...
    