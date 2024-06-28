from typing import TYPE_CHECKING, Optional, Union, ClassVar

if TYPE_CHECKING:
    from .static_library import StaticLibrary
    from .precompiled_header import PreCompiledHeader
    from .executable import Executable



class Register:
    __instance: ClassVar[Optional[Register]]
    precompiled_headers: list[PreCompiledHeader]
    static_libraries: list[StaticLibrary]
    executables: list[Executable]

    def __init__(self) -> None: ...
    @staticmethod
    def submit(other: Union[PreCompiledHeader, StaticLibrary, Executable]) -> None: ...
