import enum

class BuildSpecFlags(enum.IntFlag):
    NONE: int

    # platforms
    WINDOWS: int
    POSIX: int
    MACOS: int

    # toolchains
    GNU: int
    CLANG: int
    MSVC: int

    # special flags
    CORE: int

    def to_str(self) -> str: ...
