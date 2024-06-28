from .logger import Logger

import subprocess


class CommandQueue:
    size = 1
    queue = []
    messages = []
    fail_messages = []

    @classmethod
    def add(cls, command, message, fail_message) -> None:
        cls.queue.append(command)
        cls.messages.append(message)
        cls.fail_messages.append(fail_message)

        if cls.size == -1:
            return

        if len(cls.queue) >= cls.size:
            cls.flush()

    @classmethod
    def flush(cls) -> None:
        if len(cls.queue) == 0:
            return

        processes: list[subprocess.Popen] = []

        for i in range(len(cls.queue)):
            Logger.info(cls.messages[i])
            processes.append(
                subprocess.Popen(
                    cls.queue.pop(0)
                )
            )

        returncodes = [process.wait() for process in processes]
        for i, returncode in enumerate(returncodes):
            if returncode != 0:
                Logger.error(cls.fail_messages[i])

        cls.queue.clear()
        cls.messages.clear()
        cls.fail_messages.clear()
