import sys


def is_windows() -> bool: 
    return "win" in sys.platform
    
    
def is_linux() -> bool: 
    return "linux" in sys.platform
    
    
def is_macos() -> bool: 
    return "darwin" in sys.platform
