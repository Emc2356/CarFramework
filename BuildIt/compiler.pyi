from typing import Union, ClassVar

from pathlib import Path
import enum

from .static_library import StaticLibrary
from .executable import Executable
from .precompiled_header import PreCompiledHeader


class Toolchain(enum.IntEnum):
    NONE: Toolchain
    GNU: Toolchain
    CLANG: Toolchain
    MSVC: Toolchain


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

    @classmethod
    def set_toolchain(cls, toolchain: Toolchain) -> None: ...
    @classmethod
    def set_cxx_standard(cls, standard: int) -> None: ...
    @classmethod
    def set_c_standard(cls, standard: int) -> None: ...
    @classmethod
    def add_define(cls, name: str, value: str | None=None) -> None: ...
    @classmethod
    def add_include_directory(cls, path: str) -> None: ...
    @classmethod
    def add_build_flags(cls, *args) -> None: ...
    @classmethod
    def add_link_flags(cls, *args) -> None: ...
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
