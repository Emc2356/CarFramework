from BuildIt.logger import Logger
from BuildIt.log_file import LogFile
from pathlib import Path

import os


class SourceFile:
    __slots__ = ("path", "watches")

    def __init__(self, path: str, watches: list=[]) -> None:
        self.path: Path = Path(path)
        self.watches: list[SourceFile] = SourceFile.from_list(watches)

        if not self.path.exists():
            Logger.error(f"file `{str(self.path)}` does not exist")

    def __str__(self) -> str:
        return str(self.path)

    def __repr__(self) -> str:
        return str(self.path)

    def read(self) -> str:
        return self.path.read_text()

    def write(self, data: str) -> None:
        self.path.write_text(data)

    def exists(self) -> bool:
        return self.path.exists()

    def was_modified(self) -> bool:
        return LogFile.get(str(self.path)) != os.path.getmtime(str(self.path))

    def object_file_upto_date(self) -> bool:
        return not self.was_modified() and self.to_object_filename().exists() and all(x.was_modified() for x in self.watches)

    def has_suffx(self, suffix) -> bool:
        return str(self.path).endswith(suffix)

    def to_object_filename(self) -> Path:
        from BuildIt.compiler import Compiler

        build_dir = Compiler.build_directory
        ret = Compiler.build_directory

        for s in Path(self.path).parts:
            if s in build_dir.parts:
                continue
            ret /= s

        return ret.with_suffix(".o")

    @staticmethod
    def from_list(objects: list) -> list["SourceFile"]:
        ret: list[SourceFile] = []
        for obj in objects:
            if isinstance(obj, str):
                ret.append(SourceFile(obj))
            elif isinstance(obj, SourceFile):
                ret.append(obj)
            else:
                Logger.error(f"unrecognized object {obj}")

        return ret