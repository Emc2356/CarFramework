import enum


class BuildSpecFlags(enum.IntFlag):
    NONE = 0

    # platforms
    WINDOWS = enum.auto()
    POSIX = enum.auto()
    MACOS = enum.auto()

    # toolchains
    GNU = enum.auto()
    CLANG = enum.auto()
    MSVC = enum.auto()

    # special flags
    CORE = enum.auto()

    def to_str(self) -> str:
        if self == BuildSpecFlags.NONE:
            return "<NONE>"
        flags = []
        if self & BuildSpecFlags.WINDOWS:
            flags.append("WINDOWS")
        if self & BuildSpecFlags.POSIX:
            flags.append("POSIX")
        if self & BuildSpecFlags.MACOS:
            flags.append("MACOS")
        if self & BuildSpecFlags.GNU:
            flags.append("GNU")
        if self & BuildSpecFlags.CLANG:
            flags.append("CLANG")
        if self & BuildSpecFlags.MSVC:
            flags.append("MSVC")
        if self & BuildSpecFlags.CORE:
            flags.append("CORE")

        return f"<{'|'.join(flags)}>"
