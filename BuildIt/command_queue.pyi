from typing import ClassVar

import enum


class CommandType(enum.Enum):
    NONE: int
    EXECUTE: int
    GATHER: int


class Command:
    __slots__ = ("cmd_type", "cmd", "message", "fail_message")
    cmd_type: CommandType
    cmd: list[str]
    message: str
    fail_message: str
    
    def __init__(self, cmd_type: CommandType, cmd: list[str], message: str, fail_message: str): ...
    

def create_gather_cmd(submit: bool=True) -> Command: ...
def create_execute_command(cmd: list[str], message: str, fail_message: str, submit: bool = True) -> Command: ...


class CommandQueue:
    size: ClassVar[int]
    queue: ClassVar[list[Command]]

    @classmethod
    def add(cls, command: Command) -> None: ...
    @classmethod
    def execute(cls) -> None: ...
    @classmethod
    def holds_any_execute_commands(cls) -> bool: ...
    