from .register import Register
from .source_file import SourceFile


class Executable:
    __slots__ = (
        "name", "sources", "extra_build_flags", "extra_link_flags",
        "include_directories", "library_directories", "libraries", "extra_defines"
    )

    def __init__(self, name, sources, extra_build_flags=None, extra_link_flags=None, extra_defines=None,
                 include_directories=None, library_directories=None, libraries=None):
        if extra_build_flags is None:
            extra_build_flags = []
        if extra_link_flags is None:
            extra_link_flags = []
        if extra_defines is None:
            extra_defines = []
        if include_directories is None:
            include_directories = []
        if library_directories is None:
            library_directories = []
        if libraries is None:
            libraries = []
        self.name = name
        self.sources = SourceFile.from_list(sources)
        self.extra_build_flags = extra_build_flags
        self.extra_link_flags = extra_link_flags
        self.include_directories = include_directories
        self.library_directories = library_directories
        self.libraries = libraries
        self.extra_defines = extra_defines

        Register.submit(self)
