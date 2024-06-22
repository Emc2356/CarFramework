from typing import Union

from BuildIt.register import Register
from BuildIt.source_file import SourceFile


class Executable:
    _registered: "Executable"

    __slots__ = (
        "name", "sources", "extra_build_flags", "extra_link_flags",
        "include_directories", "library_directories", "libraries", "extra_defines"
    )
    def __init__(self, name: str, sources: list[str | SourceFile],
                 extra_build_flags: list[str]=[], extra_link_flags: list[str]=[], extra_defines: list[tuple[str] | tuple[str, str]]=[],
                 include_directories: list[str]=[], library_directories: list[str]=[], libraries: list[str]=[]):
        self.name: str = name
        self.sources: list[SourceFile] = SourceFile.from_list(sources)
        self.extra_build_flags: list[str] = extra_build_flags
        self.extra_link_flags: list[str] = extra_link_flags
        self.include_directories: list[str] = include_directories
        self.library_directories: list[str] = library_directories
        self.libraries: list[str] = libraries
        self.extra_defines: list[tuple[str] | tuple[str, str]] = extra_defines

        Register.submit(self)
