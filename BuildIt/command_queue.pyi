from typing import ClassVar


class CommandQueue:
    size: ClassVar[int]
    queue: ClassVar[list[list[str]]]
    messages: ClassVar[list[str]]
    fail_messages: ClassVar[list[str]]

    @classmethod
    def add(cls, command: list[str], message: str, fail_message: str) -> None: ...
    @classmethod
    def flush(cls) -> None: ...
