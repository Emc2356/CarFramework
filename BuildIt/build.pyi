from typing import Callable, Any, ClassVar, Optional

from .build_spec_flags import BuildSpecFlags


class Functions:
    functions: list[tuple[BuildSpecFlags, Callable[[], Any]]]
    def __init(self) -> None: ...
    @staticmethod
    def add_function(flags: BuildSpecFlags, func) -> None: ...
    @staticmethod
    def execute() -> None: ...

def build() -> None: ...


# not really fast as it needs to recursivly check all of the files
def rebuild_if_includes_changed() -> None: ...
