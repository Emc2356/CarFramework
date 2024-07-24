from typing import Union, ClassVar

from pathlib import Path
import enum

import os

from .static_library import StaticLibrary
from .executable import Executable
from .precompiled_header import PreCompiledHeader


class Toolchain(enum.IntEnum):
    NONE: Toolchain
    GNU: Toolchain
    CLANG: Toolchain
    MSVC: Toolchain
    

def set_toolchain(toolchain: Toolchain) -> None: ...
def set_cxx_standard(standard: int) -> None: ...
def set_c_standard(standard: int) -> None: ...
def add_define(name: str, value: str | None=None) -> None: ...
def add_include_directory(path: str) -> None: ...
def add_build_flags(*flags) -> None: ...
def add_link_flags(*flags) -> None: ...
def is_release() -> bool: ...
def force_release() -> None: ...


class Compiler:
    toolchain: ClassVar[Toolchain]
    c_compiler: ClassVar[str]
    cxx_compiler: ClassVar[str]
    linker: ClassVar[str]
    build_directory: ClassVar[Path]
    c_standard: ClassVar[int]
    cxx_standard: ClassVar[int]
    build_flags: ClassVar[list[str]]
    link_flags: ClassVar[list[str]]
    defines: ClassVar[dict[str, str | None]]
    include_directories: ClassVar[list[str]]
    is_release: ClassVar[bool]

    @classmethod
    def construct_build_command_gnu(
        cls,
        other: Union[StaticLibrary, Executable, PreCompiledHeader],
        ignore_pch: bool
    ) -> tuple[list[str], list[str]]: ...
    @classmethod
    def construct_build_command_clang(
        cls,
        other: Union[StaticLibrary, Executable, PreCompiledHeader],
        ignore_pch: bool
    ) -> tuple[list[str], list[str]]: ...
    @classmethod
    def construct_build_command(
        cls,
        other: Union[StaticLibrary, Executable, PreCompiledHeader],
        ignore_pch: bool=False
    ) -> tuple[list[str], list[str]]: ...


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