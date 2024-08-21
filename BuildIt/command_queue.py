from .logger import Logger

import enum

import subprocess

import time
import sys
import os
import io



def update_terminal(lines: list[str], number_of_lines_to_clear: int) -> None:
    for _ in range(number_of_lines_to_clear):
        print("\x1b[1A\x1b[2K", end="") # move up cursor and delete whole line
    for line in lines:
        print(line)


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
        
        # command, proccess
        processes: list[tuple[Command, subprocess.Popen]] = []
        
        all_cmd_count = len(cls.queue) - cls.gather_cmd_count
        
        terminal_lines_to_clear: int = 0
        want_to_update_terminal: bool = False
        tasks_completed: int = 0
        
        cls.gather_cmd_count = 0
        loop_running = True
        should_exit = False
        is_gathering = False
        
        perma_lines: list[str] = []
        while loop_running:
            done_tasks = []
            for i, (command, process) in enumerate(processes):
                result = process.poll()
                
                if result is None:
                    continue
                
                possible_msg: str = process.stdout.read()
                if possible_msg != "":
                    perma_lines.append(f"[INFO] {command.message}\n")
                    perma_lines.append(possible_msg)
                
                # process is done
                if result != 0:
                    for _, process in processes:
                        process.terminate()
                    
                    update_terminal([], terminal_lines_to_clear)
                    
                    for line in perma_lines:
                        print(line, end="")
                    perma_lines.clear()
                    sys.stdout.flush()
                
                    Logger.error(command.fail_message)
                else:
                    done_tasks.append(i)
                    
            for i in reversed(done_tasks):
                possible_msg: str = processes[i][1].stdout.read()
                if possible_msg != "":
                    perma_lines.append(f"[INFO] {processes[i][0].message}\n")
                    perma_lines.append(possible_msg)
                
                want_to_update_terminal = True
                tasks_completed += 1
                processes.pop(i)
            
            while len(processes) < cls.size:
                if is_gathering:
                    if len(processes) == 0:
                        is_gathering = False
                    else:
                        break
                
                if should_exit:
                    loop_running = False
                    break
                        
                command = cls.aquire_next_command()

                if command == None:
                    if should_exit:
                        loop_running = False
                    should_exit = True
                    
                    is_gathering = True
                    processes.clear()
                    break
                elif command.cmd_type == CommandType.GATHER:
                    is_gathering = True
                else:
                    processes.append((command, subprocess.Popen(command.cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)))            
                    want_to_update_terminal = True
            
            if want_to_update_terminal:
                terminal_lines = []
                for i, (cmd, process) in enumerate(processes):
                    terminal_lines.append(f"[{tasks_completed + i + 1}/{all_cmd_count}] {cmd.message}")
            
                update_terminal([], terminal_lines_to_clear)
                
                for line in perma_lines:
                    print(line, end="")
                perma_lines.clear()
                sys.stdout.flush()
                
                update_terminal(terminal_lines, 0)
                terminal_lines_to_clear = len(terminal_lines)
                
                want_to_update_terminal = False
            
            # poll each process 20 times per second
            time.sleep(0.05)
