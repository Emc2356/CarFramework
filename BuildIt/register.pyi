from typing import TYPE_CHECKING, Optional, Union, ClassVar

if TYPE_CHECKING:
    from .static_library import StaticLibrary
    from .executable import Executable



class Register:
    __instance: ClassVar[Optional[Register]]
    static_libraries: list[StaticLibrary]
    executables: list[Executable]

    def __init__(self) -> None: ...
    @staticmethod
    def submit(other: Union[StaticLibrary, Executable]) -> None: ...
