from typing import Union

from pathlib import Path
import enum

from .register import Register
from .logger import Logger
from .static_library import StaticLibrary
from .executable import Executable


class Toolchain(enum.IntEnum):
    NONE = 0
    GNU = enum.auto()
    CLANG = enum.auto()
    MSVC = enum.auto()


class Compiler:
    toolchain: Toolchain = Toolchain.GNU
    c_compiler = "gcc"
    cxx_compiler = "g++"
    linker = "g++"
    build_directory = Path.cwd() / "build"
    c_standard = 17
    cxx_standard = 17
    build_flags = []
    link_flags = []
    defines = {}
    include_directories = []
    is_release = False

    @classmethod
    def construct_build_command_gnu(cls, other, ignore_pch):
        command_c: list[str] = [
            cls.c_compiler,
            f"-std=c{cls.c_standard}",
        ]
        command_cxx: list[str] = [
            cls.cxx_compiler,
            f"-std=c++{cls.cxx_standard}",
        ]

        common: list[str] = ["-c"] + other.extra_build_flags + cls.build_flags
        
        if cls.is_release or (hasattr(other, "is_optimized") and other.is_optimized):
            common.append("-O3")
        else:
            common.append("-ggdb")

        for name, value in cls.defines.items():
            common.append("-D")

            if value is not None:
                common.append(f"{name}={value}")
            else:
                common.append(name)

        for define in other.extra_defines:
            name = define[0]
            value = define[1] if len(define) > 1 else None

            common.append("-D")

            if value is None:
                common.append(name)
            else:
                common.append(f"{name}={value}")

        if hasattr(other, "include_directories"):
            for include_directory in other.include_directories:
                common.append("-I")
                common.append(include_directory)

        for include_directory in cls.include_directories:
            common.append("-I")
            common.append(include_directory)

        command_c.extend(common)
        command_cxx.extend(common)

        if not ignore_pch:
            if hasattr(other, "attached_precompiled_headers"):
                for header_soure in other.attached_precompiled_headers:
                    command_cxx.append("-include")
                    command_cxx.append(f"{header_soure}")

                    if header_soure.has_suffix(".h"):
                        command_c.append("-include")
                        command_c.append(f"{header_soure}")

        if hasattr(other, "is_forced_cxx") and other.is_forced_cxx:
            command_c = command_cxx

        return command_c, command_cxx

    @classmethod
    def construct_build_command_clang(cls, other, ignore_pch):
        command_c: list[str] = [
            cls.c_compiler,
            f"-std=c{cls.c_standard}",
        ]
        command_cxx: list[str] = [
            cls.cxx_compiler,
            f"-std=c++{cls.cxx_standard}",
        ]

        common: list[str] = ["-c"] + other.extra_build_flags + cls.build_flags

        if cls.is_release or (hasattr(other, "is_optimized") and other.is_optimized):
            common.append("-O3")
        else:
            common.append("-ggdb")

        for name, value in cls.defines.items():
            common.append("-D")

            if value is not None:
                common.append(f"{name}={value}")
            else:
                common.append(name)

        for define in other.extra_defines:
            name = define[0]
            value = define[1] if len(define) > 1 else None

            common.append("-D")

            if value is None:
                common.append(name)
            else:
                common.append(f"{name}={value}")

        if hasattr(other, "include_directories"):
            for include_directory in other.include_directories:
                common.append("-I")
                common.append(include_directory)

        for include_directory in cls.include_directories:
            common.append("-I")
            common.append(include_directory)

        command_c.extend(common)
        command_cxx.extend(common)

        if not ignore_pch:
            if hasattr(other, "attached_precompiled_headers"):
                for header_soure in other.attached_precompiled_headers:
                    command_cxx.append("-include-pch")
                    command_cxx.append(f"{header_soure}.pch")

                    if header_soure.has_suffix(".h"):
                        command_c.append("-include-pch")
                        command_c.append(f"{header_soure}.pch")

        if hasattr(other, "is_forced_cxx") and other.is_forced_cxx:
            command_c = command_cxx

        return command_c, command_cxx

    @classmethod
    def construct_build_command(cls, other, ignore_pch=False):
        """
        returns the c build command and the c++ build command in a tuple
        """
        if cls.toolchain == Toolchain.GNU:
            return cls.construct_build_command_gnu(other, ignore_pch)
        elif cls.toolchain == Toolchain.CLANG:
            return cls.construct_build_command_clang(other, ignore_pch)
        else:
            Logger.error(f"Internal error at construct_build_command, should have failed earlier")


def set_toolchain(toolchain):
    if toolchain == Toolchain.GNU:
        Compiler.toolchain = Toolchain.GNU
        Compiler.c_compiler = "gcc"
        Compiler.cxx_compiler = "g++"
        Compiler.linker = "g++"
    elif toolchain == Toolchain.CLANG:
        Compiler.toolchain = Toolchain.CLANG
        Compiler.c_compiler = "clang"
        Compiler.cxx_compiler = "clang++"
        Compiler.linker = "clang++"
    elif toolchain == Toolchain.MSVC:
        Logger.error("`msvc` toolchain not implemented yet")
    else:
        Logger.error(f"toolchain `{toolchain}` is not supported")


def set_cxx_standard(standard):
    Compiler.cxx_standard = standard


def set_c_standard(standard):
    Compiler.c_standard = standard


def add_define(name, value=None):
    if name in Compiler.defines.keys():
        Logger.warn(f"redefining `{name}` macro")
    Compiler.defines[name] = value


def add_include_directory(path):
    Compiler.include_directories.append(str(path))


def add_build_flags(*flags) -> None:
    for arg in flags:
        if isinstance(arg, str):
            Compiler.build_flags.append(arg)
        elif isinstance(arg, (list, tuple)):
            add_build_flags(arg)
        else:
            Logger.error(f"Expected a series of strings `{arg}`")


def add_link_flags(*flags) -> None:
    for arg in flags:
        if isinstance(arg, str):
            Compiler.link_flags.append(arg)
        elif isinstance(arg, (list, tuple)):
            add_link_flags(arg)
        else:
            Logger.error(f"Expected a series of strings `{arg}`")
            

def is_release() -> bool:
    return Compiler.is_release


def force_release() -> None:
    Compiler.is_release = True

            
__all__ = [
    "Toolchain",
    "set_toolchain",
    "set_cxx_standard",
    "set_c_standard",
    "add_define",
    "add_include_directory",
    "add_build_flags",
    "add_link_flags",
    "is_release",
    "force_release"
]