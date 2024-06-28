from pathlib import Path
from .source_file import SourceFile
from .register import Register


class StaticLibrary:
    __slots__ = (
        "name", "out_filepath", "sources", "extra_build_flags", "extra_defines", "depends_on", "include_directories"
    )

    def __init__(self, name, out_filepath, sources,
                 extra_build_flags=None, extra_defines=None, depends_on=None, include_directories=None):
        if include_directories is None:
            include_directories = []
        if depends_on is None:
            depends_on = []
        if extra_defines is None:
            extra_defines = []
        if extra_build_flags is None:
            extra_build_flags = []

        self.name = name
        self.out_filepath = Path(out_filepath)
        self.sources = SourceFile.from_list(sources)
        self.extra_build_flags = extra_build_flags
        self.extra_defines = extra_defines
        self.depends_on = depends_on
        self.include_directories = include_directories

        Register.submit(self)
