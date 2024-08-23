from typing import Callable, Any, overload

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


@overload
def buildspec(flags: BuildSpecFlags) -> Callable: ...
@overload
def buildspec(func: Callable[[], Any]) -> Callable[[Callable[[], Any]], Any]: ...
@overload
def buildspec(flags: BuildSpecFlags, condition: bool) -> Callable[[Callable[[], Any]], Any]: ...
@overload
def buildspec(flags: BuildSpecFlags, condition_callback: Callable[[], bool]) -> Callable[[Callable[[], Any]], Any]: ...
