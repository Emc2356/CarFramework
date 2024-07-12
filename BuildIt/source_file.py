from .logger import Logger
from .log_file import LogFile
from pathlib import Path

import os


class SourceFile:
    __slots__ = ("path", "watches")

    def __init__(self, path, watches=None) -> None:
        if watches is None:
            watches = []
        self.path = Path(path)
        self.watches = SourceFile.from_list(watches)

        if not self.path.exists():
            Logger.error(f"file `{str(self.path)}` does not exist")

    def __str__(self):
        return str(self.path)

    def __repr__(self):
        return str(self.path)

    def read(self):
        return self.path.read_text()

    def write(self, data):
        self.path.write_text(data)

    def exists(self):
        return self.path.exists()

    def was_modified(self):
        return LogFile.get(str(self.path)) != os.path.getmtime(str(self.path))

    def object_file_upto_date(self):
        return (
            not self.was_modified() and
            self.to_object_filename().exists() and
            all(x.was_modified() for x in self.watches)
        )

    def has_suffix(self, suffix):
        return str(self.path).endswith(suffix)

    def to_object_filename(self):
        from BuildIt.compiler import Compiler

        ret = Path(
            os.path.commonpath([Compiler.build_directory, str(self.path.absolute())]),
            "build",
            str(self.path)
        )

        return ret.with_suffix(".o")

    @staticmethod
    def from_list(objects):
        ret: list[SourceFile] = []
        for obj in objects:
            if isinstance(obj, str):
                ret.append(SourceFile(obj))
            elif isinstance(obj, SourceFile):
                ret.append(obj)
            else:
                Logger.error(f"unrecognized object {obj}")

        return ret
