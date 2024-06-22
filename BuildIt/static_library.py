from pathlib import Path
from BuildIt.source_file import SourceFile
from BuildIt.register import Register


class StaticLibrary:
    __slots__ = ("name", "out_filepath", "sources", "extra_build_flags", "extra_defines", "depends_on", "include_directories")
    def __init__(self, name: str, out_filepath: str, sources: list[str | SourceFile],
                 extra_build_flags: list[str]=[], extra_defines: list[tuple[str, str] | tuple[str]]=[],
                 depends_on: list[str]=[], include_directories: list[str]=[]):
        self.name: str = name
        self.out_filepath: Path = Path(out_filepath)
        self.sources: list[SourceFile] = SourceFile.from_list(sources)
        self.extra_build_flags: list[str] = extra_build_flags
        self.extra_defines: list[tuple[str, str] | tuple[str]] = extra_defines
        self.depends_on: list[str] = depends_on
        self.include_directories: list[str] = include_directories

        Register.submit(self)