import enum

class BuildSpecFlags(enum.IntFlag):
    NONE: BuildSpecFlags

    # platforms
    WINDOWS: BuildSpecFlags
    LINUX: BuildSpecFlags
    MACOS: BuildSpecFlags
    
    ANY_PLATFORM: BuildSpecFlags

    # toolchains
    GNU: BuildSpecFlags
    CLANG: BuildSpecFlags
    MSVC: BuildSpecFlags
    
    ANY_TOOLCHAIN: BuildSpecFlags
    
    ANY: BuildSpecFlags

    # special flags
    CORE: BuildSpecFlags

    def to_str(self) -> str: ...
