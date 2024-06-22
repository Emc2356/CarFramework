from typing import Union

from pathlib import Path

from BuildIt.register import Register
from BuildIt.logger import Logger
from BuildIt.static_library import StaticLibrary
from BuildIt.executable import Executable
from BuildIt.precompiled_header import PreCompiledHeader


class Compiler:
    toolchain: str = "gnu"
    c_compiler: str = "gcc"
    cxx_compiler: str = "g++"
    linker: str = "g++"
    build_directory: Path = Path.cwd() / "build"
    c_standard: int = 17
    cxx_standard: int = 17
    build_flags: list[str] = []
    link_flags: list[str] = []
    defines: dict[str, str | None] = {}

    @classmethod
    def set_toolchain(cls, toolchain: str) -> None:
        if toolchain.lower() == "gnu":
            cls.toolchain = "gnu"
            cls.c_compiler = "gcc"
            cls.cxx_compiler = "g++"
            cls.linker = "g++"
        elif toolchain.lower() == "clang":
            Logger.error("`clang` toolchain not implemented yet")
        elif toolchain.lower() == "msvc":
            Logger.error("`msvc` toolchain not implemented yet")
        else:
            Logger.error(f"toolchain `{toolchain}` is not supported")

    @classmethod
    def set_cxx_standard(cls, standard: int) -> None:
        cls.cxx_standard = standard

    @classmethod
    def set_c_standard(cls, standard: int) -> None:
        cls.c_standard = standard

    @classmethod
    def add_define(cls, name: str, value: str | None=None) -> None:
        if name in cls.defines.keys():
            Logger.warn(f"redefining `{name}` macro")
        cls.defines[name] = value

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
    def construct_build_command_gnu(cls, other: Union[StaticLibrary, Executable, PreCompiledHeader]) -> tuple[list[str], list[str]]:
        command_c: list[str] = [
            cls.c_compiler,
            f"-std=gnu{cls.c_standard}",
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

        command_c.extend(common)
        command_cxx.extend(common)


        for precompiled_header in Register().precompiled_headers:
            command_cxx.append("-include")
            command_cxx.append(f"{precompiled_header.source}")

            if precompiled_header.source.has_suffx(".h"):
                command_c.append("-include")
                command_c.append(f"{precompiled_header.source}")

        return command_c, command_cxx

    @classmethod
    def construct_build_command(cls, other: Union[StaticLibrary, Executable, PreCompiledHeader]) -> tuple[list[str], list[str]]:
        """
        returns the c build command and the c++ build command in a tuple 
        """
        if cls.toolchain == "gnu":
            return cls.construct_build_command_gnu(other)
        else:
            Logger.error(f"Internal error at construct_build_command, should have failed earlier")
