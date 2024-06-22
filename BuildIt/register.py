from __future__ import annotations
from typing import TYPE_CHECKING, Optional, Union

from BuildIt.logger import Logger

if TYPE_CHECKING:
    from BuildIt.static_library import StaticLibrary
    from BuildIt.precompiled_header import PreCompiledHeader
    from BuildIt.executable import Executable




class Register:
    __slots__ = ("precompiled_headers", "static_libraries", "executables")

    __instance: Optional["Register"] = None

    def __new__(cls, *args, **kwargs) -> "Register":
        if cls.__instance is None:
            cls.__instance = super(Register, cls).__new__(cls, *args, **kwargs)
            cls.__instance.__init()
        return cls.__instance

    def __init(self):
        self.precompiled_headers: list["PreCompiledHeader"] = []
        self.static_libraries: list["StaticLibrary"] = []
        self.executables: list["Executable"] = []

    @staticmethod
    def submit(other: Union["PreCompiledHeader", "StaticLibrary", "Executable"]) -> None:
        from BuildIt.static_library import StaticLibrary
        from BuildIt.precompiled_header import PreCompiledHeader
        from BuildIt.executable import Executable

        if isinstance(other, StaticLibrary):
            Register().static_libraries.append(other)
        elif isinstance(other, PreCompiledHeader):
            Register().precompiled_headers.append(other)
        elif isinstance(other, Executable):
            Register().executables.append(other)
        else:
            Logger.error(f"internal error: `{other}` in `register.py`")
