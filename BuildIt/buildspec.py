import sys
from typing import Callable

import enum


class BuildSpecFlags(enum.IntFlag):
    NONE = 0

    # platforms
    WINDOWS = enum.auto()
    LINUX = enum.auto()
    MACOS = enum.auto()
    
    ANY_PLATFORM = WINDOWS | LINUX | MACOS

    # toolchains
    GNU = enum.auto()
    CLANG = enum.auto()
    MSVC = enum.auto()
    
    ANY_TOOLCHAIN = GNU | CLANG | MSVC

    ANY = WINDOWS | LINUX | MACOS | GNU | CLANG | MSVC

    # special flags
    CORE = enum.auto()

    def to_str(self) -> str:
        if self == BuildSpecFlags.NONE:
            return "<NONE>"
        flags = []
        if self & BuildSpecFlags.WINDOWS:
            flags.append("WINDOWS")
        if self & BuildSpecFlags.LINUX:
            flags.append("LINUX")
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


def get_build_flags_from_function_name(func: Callable) -> BuildSpecFlags:
    flags: BuildSpecFlags = BuildSpecFlags.NONE
    name = func.__name__

    flags |= BuildSpecFlags.WINDOWS if "windows" in name else 0
    flags |= BuildSpecFlags.LINUX if "linux" in name else 0
    flags |= BuildSpecFlags.MACOS if "macos" in name else 0
    flags |= BuildSpecFlags.GNU if "gnu" in name else 0
    flags |= BuildSpecFlags.CLANG if "clang" in name else 0
    flags |= BuildSpecFlags.MSVC if "msvc" in name else 0
    flags |= BuildSpecFlags.CORE if "core" in name else 0

    return flags


def buildspec(*args):
    from .build import Functions
    
    is_windows: bool = "win" in sys.platform
    is_linux: bool = "linux" in sys.platform
    is_macos: bool = "darwin" in sys.platform
    # @buildspec(flags)
    # @buildspec
    if len(args) == 1:  # either a flags or the function
        flags_or_function = args[0]
        if callable(flags_or_function):  # a function
            flags = get_build_flags_from_function_name(flags_or_function)
            if ((flags & BuildSpecFlags.WINDOWS and is_windows) or
                    (flags & BuildSpecFlags.LINUX and is_linux) or
                    (flags & BuildSpecFlags.MACOS and is_macos)):
                Functions.add_function(flags, flags_or_function)

            return flags_or_function

        # here we have just flags
        def inner(func):
            flags = args[0]
            if ((flags & BuildSpecFlags.WINDOWS and is_windows) or
                    (flags & BuildSpecFlags.LINUX and is_linux) or
                    (flags & BuildSpecFlags.MACOS and is_macos)):
                Functions.add_function(flags, func)

            return func

        return inner
    # @buildspec(flags, bool)
    # @buildspec(flags, bool_callback)
    elif len(args) == 2:  # a flags and a condition
        flags: BuildSpecFlags = args[0]
        condition_or_callback: bool | Callable[[], bool] = args[1]
        condition: bool = condition_or_callback() if callable(condition_or_callback) else condition_or_callback

        if not isinstance(flags, BuildSpecFlags):
            raise TypeError(f"first argument must be a BuildSpecFlag")

        if condition:
            def inner(func):
                if ((flags & BuildSpecFlags.WINDOWS and is_windows) or
                        (flags & BuildSpecFlags.LINUX and is_linux) or
                        (flags & BuildSpecFlags.MACOS and is_macos)):
                            Functions.add_function(flags, func)

                return func

            return inner

        def inner(func):
            return func

        return inner
    else:
        raise TypeError(f"function takes at most 2 arguments ({len(args)} was given)")
