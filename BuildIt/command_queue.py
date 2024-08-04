from .logger import Logger

import enum

import subprocess

import time
import sys
import os
import io


# https://github.com/termcolor/termcolor/blob/main/src/termcolor/termcolor.py
def _can_do_colour() -> bool:
    """Check env vars and for tty/dumb terminal"""
    # First check overrides:
    # "User-level configuration files and per-instance command-line arguments should
    # override $NO_COLOR. A user should be able to export $NO_COLOR in their shell
    # configuration file as a default, but configure a specific program in its
    # configuration file to specifically enable color."
    # https://no-color.org
    if "ANSI_COLORS_DISABLED" in os.environ:
        return False
    if "NO_COLOR" in os.environ:
        return False
    if "FORCE_COLOR" in os.environ:
        return True

    # Then check system:
    if os.environ.get("TERM") == "dumb":
        return False
    if not hasattr(sys.stdout, "fileno"):
        return False

    try:
        return os.isatty(sys.stdout.fileno())
    except io.UnsupportedOperation:
        return sys.stdout.isatty()



class CommandType(enum.Enum):
    NONE = 0
    EXECUTE = enum.auto()
    GATHER = enum.auto()


def create_gather_cmd(submit=True):
    cmd = Command(CommandType.GATHER, [], "", "")
    if submit:
        CommandQueue.add(cmd)
    return cmd


def create_execute_command(cmd, message, fail_message, submit=True):
    cmd = Command(CommandType.EXECUTE, cmd, message, fail_message)
    if submit:
        CommandQueue.add(cmd)
    return cmd


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
    def aquire_next_command(cls) -> Command | None:
        if len(cls.queue) == 0:
            return None
        
        return cls.queue.pop(0)

    @classmethod
    def execute(cls) -> None:
        # exit(_can_do_colour())
        if len(cls.queue) == 0 or cls.gather_cmd_count == len(cls.queue):
            return
        
        processes: list[tuple[Command, subprocess.Popen]] = []
        
        current_count = 0
        all_cmd_count = len(cls.queue) - cls.gather_cmd_count
        
        cls.gather_cmd_count = 0
        loop_running = True
        while loop_running:
            done_tasks = []
            for i, (command, process) in enumerate(processes):
                result = process.poll()
                if result is None:
                    continue
                    
                if result != 0:
                    for _, process in processes:
                        process.terminate()
                    
                    Logger.error(command.fail_message)
                else:
                    done_tasks.append(i)
                    
            for i in reversed(done_tasks):
                processes.pop(i)

            while len(processes) < cls.size:
                command = cls.aquire_next_command()
                
                if command == None:
                    for command, process in processes:
                        if process.wait() != 0:
                            Logger.error(command.fail_message)

                    loop_running = False
                    processes.clear()
                    break
                elif command.cmd_type == CommandType.GATHER:
                    for command, process in processes:
                        if process.wait() != 0:
                            Logger.error(command.fail_message)
                    processes.clear()
                else:
                    Logger.info(f"[{current_count + 1}/{all_cmd_count}] {command.message}")
                    processes.append((command, subprocess.Popen(command.cmd)))            
                    current_count += 1
            
            # sleep 16ms to not max out the cpu
            time.sleep(0.016)

        # catch any struglers
        for command, process in processes:
                if process.wait() != 0:
                    Logger.error(command.fail_message)
