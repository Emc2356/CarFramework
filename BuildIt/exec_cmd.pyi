class ExecResult:
    __slots__ = ("args", "returncode", "stdout", "stderr")
    args: list[str]
    returncode: int
    stdout: str
    stderr: str
    
    def __bool__(self) -> bool: ...


def exec_cmd(*command: str, is_python: bool=False, cwd: str="") -> ExecResult: ...
