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
