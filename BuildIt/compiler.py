from typing import Union

from pathlib import Path
import enum

from .register import Register
from .logger import Logger
from .static_library import StaticLibrary
from .executable import Executable
from .precompiled_header import PreCompiledHeader


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

    @classmethod
    def set_toolchain(cls, toolchain) -> None:
        if toolchain == Toolchain.GNU:
            cls.toolchain = Toolchain.GNU
            cls.c_compiler = "gcc"
            cls.cxx_compiler = "g++"
            cls.linker = "g++"
        elif toolchain == Toolchain.CLANG:
            cls.toolchain = Toolchain.CLANG
            cls.c_compiler = "clang"
            cls.cxx_compiler = "clang++"
            cls.linker = "clang++"
        elif toolchain == Toolchain.MSVC:
            Logger.error("`msvc` toolchain not implemented yet")
        else:
            Logger.error(f"toolchain `{toolchain}` is not supported")

    @classmethod
    def set_cxx_standard(cls, standard) -> None:
        cls.cxx_standard = standard

    @classmethod
    def set_c_standard(cls, standard) -> None:
        cls.c_standard = standard

    @classmethod
    def add_define(cls, name, value=None) -> None:
        if name in cls.defines.keys():
            Logger.warn(f"redefining `{name}` macro")
        cls.defines[name] = value

    @classmethod
    def add_include_directory(cls, path):
        cls.include_directories.append(str(path))

    @classmethod
    def add_build_flags(cls, *args) -> None:
        for arg in args:
            if isinstance(arg, str):
                cls.build_flags.append(arg)
            elif isinstance(arg, (list, tuple)):
                cls.add_build_flags(arg)
            else:
                Logger.error(f"Expected a series of strings `{arg}`")

    @classmethod
    def add_link_flags(cls, *args) -> None:
        for arg in args:
            if isinstance(arg, str):
                cls.link_flags.append(arg)
            elif isinstance(arg, (list, tuple)):
                cls.add_link_flags(arg)
            else:
                Logger.error(f"Expected a series of strings `{arg}`")

    @classmethod
    def construct_build_command_gnu(cls, other, ignore_pch):
        command_c: list[str] = [
            cls.c_compiler,
            f"-std={cls.c_standard}",
        ]
        command_cxx: list[str] = [
            cls.cxx_compiler,
            f"-std=c++{cls.cxx_standard}",
        ]

        common: list[str] = ["-c"] + other.extra_build_flags + cls.build_flags

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
            for precompiled_header in Register().precompiled_headers:
                command_cxx.append("-include")
                command_cxx.append(f"{precompiled_header.source}")

                if precompiled_header.source.has_suffix(".h"):
                    command_c.append("-include")
                    command_c.append(f"{precompiled_header.source}")

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
            for precompiled_header in Register().precompiled_headers:
                command_cxx.append("-include-pch")
                command_cxx.append(f"{precompiled_header.source}.pch")

                if precompiled_header.source.has_suffix(".h"):
                    command_c.append("-include")
                    command_c.append(f"{precompiled_header.source}")

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
