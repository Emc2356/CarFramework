from pathlib import Path
import os


class LogFile:
    data = {}
    log_file = Path.cwd() / "build" / "log.log"
    is_loaded = False

    @classmethod
    def update(cls, file):
        if not cls.is_loaded:
            cls.load()
            cls.is_loaded = True
        cls.set(str(file), os.path.getmtime(str(file)))

    @classmethod
    def set(cls, name, time):
        if not cls.is_loaded:
            cls.load()
            cls.is_loaded = True
        cls.data[name] = time
        cls.save()

    @classmethod
    def get(cls, name):
        if not cls.is_loaded:
            cls.load()
            cls.is_loaded = True
        return cls.data.get(name, 0)

    @classmethod
    def load(cls):
        cls.log_file.parent.mkdir(exist_ok=True)
        cls.log_file.touch(exist_ok=True)

        lines = cls.log_file.read_text().splitlines()

        if len(lines) % 2 == 1:
            cls.data = {}

        for i, j in zip(range(0, len(lines), 2), range(1, len(lines), 2)):
            cls.data[lines[i]] = float(lines[j])

    @classmethod
    def save(cls) :
        with cls.log_file.open("w") as f:
            for name, time in cls.data.items():
                f.write(f"{name}\n{time}\n")
