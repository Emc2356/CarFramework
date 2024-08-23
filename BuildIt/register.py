from typing import TYPE_CHECKING

from .logger import Logger

if TYPE_CHECKING:
    from .static_library import StaticLibrary
    from .executable import Executable


class Register:
    __slots__ = ("static_libraries", "executables")

    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(Register, cls).__new__(cls, *args, **kwargs)
            cls._instance.__init()
        return cls._instance

    def __init(self) -> None:
        self.static_libraries = []
        self.executables = []

    @staticmethod
    def submit(other):
        from BuildIt.static_library import StaticLibrary
        from BuildIt.executable import Executable

        if isinstance(other, StaticLibrary):
            Register().static_libraries.append(other)
        elif isinstance(other, Executable):
            Register().executables.append(other)
        else:
            Logger.error(f"internal error: `{other}` in `register.py`")
