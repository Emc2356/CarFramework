import enum

class BuildSpecFlags(enum.IntFlag):
    NONE: BuildSpecFlags

    # platforms
    WINDOWS: BuildSpecFlags
    POSIX: BuildSpecFlags
    MACOS: BuildSpecFlags

    # toolchains
    GNU: BuildSpecFlags
    CLANG: BuildSpecFlags
    MSVC: BuildSpecFlags

    # special flags
    CORE: BuildSpecFlags

    def to_str(self) -> str: ...
