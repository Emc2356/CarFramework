from typing import ClassVar

from pathlib import Path


class LogFile:
    data: ClassVar[dict[str, float]]
    log_file: ClassVar[Path]
    is_loaded: ClassVar[bool]

    @classmethod
    def update(cls, file) -> None: ...
    @classmethod
    def set(cls, name: str, time: float) -> None: ...
    @classmethod
    def get(cls, name: str) -> float: ...
    @classmethod
    def load(cls) -> None: ...
    @classmethod
    def save(cls) -> None: ...
