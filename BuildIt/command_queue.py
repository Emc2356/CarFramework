from .logger import Logger

import enum

import subprocess


class CommandType(enum.Enum):
    NONE = 0
    EXECUTE = enum.auto()
    GATHER = enum.auto()


def create_gather_cmd():
    CommandQueue.add(Command(CommandType.GATHER, [], "", ""))


def create_execute_command(cmd, message, fail_message):
    CommandQueue.add(Command(CommandType.EXECUTE, cmd, message, fail_message))


class Command:
    __slots__ = ("cmd_type", "cmd", "message", "fail_message")
    
    def __init__(self, cmd_type, cmd, message, fail_message):
        self.cmd_type = cmd_type
        self.cmd = cmd
        self.message = message
        self.fail_message = fail_message


class CommandQueue:
    size = 1
    queue = []
    gather_cmd_count = 0

    @classmethod
    def add(cls, command) -> None:
        cls.queue.append(command)
        
        if command.cmd_type == CommandType.GATHER:
            cls.gather_cmd_count += 1

    @classmethod
    def execute(cls) -> None:
        processes: list[tuple[Command, subprocess.Popen]] = []
        
        current_count = 0
        all_cmd_count = len(cls.queue) - cls.gather_cmd_count
        cls.gather_cmd_count = 0
        while len(cls.queue):
            command = cls.queue.pop(0)
            
            if command.cmd_type == CommandType.EXECUTE:
                Logger.info(f"[{current_count + 1}/{all_cmd_count}] {command.message}")
                processes.append((command, subprocess.Popen(command.cmd)))
                current_count += 1
            
            if (len(processes) >= cls.size and cls.size != -1) or command.cmd_type == CommandType.GATHER:
                for command, process in processes:
                    if process.wait() != 0:
                        Logger.error(command.fail_message)
                
                processes.clear()
