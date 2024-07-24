from .compiler import (
    Toolchain,
    set_toolchain,
    set_cxx_standard,
    set_c_standard,
    add_define,
    add_include_directory,
    add_build_flags,
    add_link_flags,
    is_release,
    force_release
)
from .executable import Executable
from .static_library import StaticLibrary
from .precompiled_header import PreCompiledHeader
from .build import build
from .decorators import buildspec
from .build_spec_flags import BuildSpecFlags
from .cli import handle_argv, unknown_argument
from .exec_cmd import exec_cmd, ExecResult


__all__ = [
    "Toolchain",
    "set_toolchain",
    "set_cxx_standard",
    "set_c_standard",
    "add_define",
    "add_include_directory",
    "add_build_flags",
    "add_link_flags",
    "is_release",
    "force_release",
    "Executable",
    "StaticLibrary",
    "PreCompiledHeader",
    "build",
    "buildspec",
    "BuildSpecFlags",
    "handle_argv",
    "unknown_argument",
    "exec_cmd",
    "ExecResult"
]
