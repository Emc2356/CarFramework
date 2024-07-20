import subprocess
import sys
import os


class ExecResult:
    __slots__ = ("args", "returncode", "stdout", "stderr")

    def __init__(self, args, returncode, stdout, stderr):
        self.args = args
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = stderr
        
    def __bool__(self):
        return bool(self.returncode)


def exec_cmd(*command, is_python=False, cwd=""):
    cmd = list(command)
    if is_python:
        cmd.insert(0, sys.executable)
    if cwd == "":
        cwd = os.getcwd()
        
    result = subprocess.run(command, cwd=cwd)

    return ExecResult(result.args, result.returncode, result.stdout, result.stderr)
    