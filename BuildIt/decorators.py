from BuildIt.logger import Logger

import sys
import os


def build_spec(platform_or_func):
    if callable(platform_or_func):
        if "win" in platform_or_func.__name__:
            if "win" in sys.platform:
                platform_or_func()
        elif "posix" in platform_or_func.__name__:
            if "linux" in sys.platform:
                platform_or_func()
        elif "osx" in platform_or_func.__name__:
            if "darwin" in sys.platform:
                platform_or_func()
        else:
            platform_or_func()
        return platform_or_func
    
    platform = str(platform_or_func)

    def inner(func):

        if platform == "any":
            func()
        elif platform == "win":
            if "win" in sys.platform:
                func()
        elif platform == "posix":
            if "linux" in sys.platform:
                func()
        elif platform == "osx":
            if "darwin" in sys.platform:
                func()
        else:
            Logger.error(f"unrecognized platform `{platform}`, valid platforms (\"any\", \"win\", \"posix\", \"osx\")")
        return func()
    
    return inner