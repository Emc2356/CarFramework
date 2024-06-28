# BuildIt
another build system to pile ontop of other builds system cause why not?

it is made in python and its language is python so you can add pre build and post build steps very easily

# Speed
Absolutely no idea, it depends on the C/C++ compiler and their implementation plus some overhead for python so propably not too slow

# Examples
```py
from BuildIt import *


@buildspec
def glad_gnu_clang():
    StaticLibrary(
        name="glad",
        out_filepath="./libraries/",
        sources=[
            "./vendor/glad/src/gl.c",
            "./vendor/glad/src/vulkan.c",
        ],
        include_directories=[
            "./vendor/glad/include/",
        ],
    )
```
here we declare a function `glad` that will get called when we set the toolchain to gnu or clang

note: `StaticLibrary` has an optional parameter `depends_on` that is a list of strings that refers to other libraries so the libraries are passed in the correct order to the linker 

this by itself wont work as we are missing the "core" function. the Core function is what is responsible for setting the toolchain, c and c++ standard and other things like that, now our program will look like this:

```py
from BuildIt import *


@buildspec
def glad_gnu_clang():
    StaticLibrary(
        name="glad",
        out_filepath="./libraries/",
        sources=[
            "./vendor/glad/src/gl.c",
            "./vendor/glad/src/vulkan.c",
        ],
        include_directories=[
            "./vendor/glad/include/",
        ],
    )


@buildspec(BuildSpecFlags.POSIX | BuildSpecFlags.WINDOWS | BuildSpecFlags.CORE, __name__ == "__main__")
def core():
    Compiler.set_toolchain(Toolchain.CLANG)
    Compiler.set_c_standard(17)
    Compiler.set_cxx_standard(17)
    Compiler.add_build_flags("-O3", "-g0")

    Compiler.add_define("DEBUG")
    Compiler.add_define("_DEBUG", "1")
```
here we see some different syntax

instead of declaring the platform or specific toolchain in the name of the function we declare it from the `buildspec` decorator, the first parameter is the flags that tells BuildIt that this will be called in posix and windows system and that this is the core function. Then we have the second parameter, it is just syntactic sugar for an if statemeant that it is the same as this:
```py
if __name__ == "__main__":
    @buildspec(BuildSpecFlags.POSIX | BuildSpecFlags.WINDOWS | BuildSpecFlags.CORE)
    def core():
        Compiler.set_toolchain(Toolchain.CLANG)
        Compiler.set_c_standard(17)
        Compiler.set_cxx_standard(17)
        Compiler.add_build_flags("-O3", "-g0")

        Compiler.add_define("DEBUG")
        Compiler.add_define("_DEBUG", "1")
```
note: the second parameter can also be a callback

note: only one core function can be called

finally making executables is extremly simple
```py
@buildspec(BuildSpecFlags.CORE | BuildSpecFlags.POSIX | BuildSpecFlags.WINDOWS, __name__ == "__main__")
def core() -> None:
    Compiler.set_toolchain(Toolchain.CLANG)
    Compiler.set_c_standard(17)
    Compiler.set_cxx_standard(17)
    Compiler.add_build_flags("-O3", "-g0")

    Executable(
        name="sandbox.out",
        sources=[
            "SandBox/src/main.cpp",
        ],
        extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
        extra_link_flags=[],
        include_directories=[
            "./Car/include/",
            "./vendor/stb/include/",
            "./vendor/glad/include/",
            "./vendor/glfw/include/",
            "./vendor/imgui/",
            "./vendor/glm/",
            "./vendor/debugbreak/",
            "./vendor/entt/src/",
        ],
        libraries=[
            "GL",
            "fmt",
        ],
        extra_defines=[
            ("GLFW_INCLUDE_NONE",),
        ]
    )
```