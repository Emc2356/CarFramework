from .source_file import SourceFile


class Executable:
    name: str
    sources: list[SourceFile]
    extra_build_flags: list[str]
    extra_link_flags: list[str]
    include_directories: list[str]
    library_directories: list[str]
    libraries: list[str]
    extra_defines: list[tuple[str] | tuple[str, str]]
    static_libraries: list[str]

    def __init__(
        self,
        name: str,
        sources: list[str | SourceFile],
        extra_build_flags: list[str]=[],
        extra_link_flags: list[str]=[],
        static_libraries: list[str]=[],
        extra_defines: list[tuple[str] | tuple[str, str]]=[],
        include_directories: list[str]=[],
        library_directories: list[str]=[],
        libraries: list[str]=[]
    ): ...
