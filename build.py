#!/usr/bin/env python

from BuildIt import *
import subprocess
import sys


@buildspec
def pch_windows_posix_gnu_clang() -> None:
    PreCompiledHeader(
        source="./Car/include/Car/Core/crpch.hpp",
        extra_build_flags=[],
        extra_defines=[]
    )


@buildspec
def glfw_posix_gnu_clang() -> None:
    StaticLibrary(
        name="glfw",
        out_filepath="./libraries/",
        sources=[
            "./vendor/glfw/src/context.c",
            "./vendor/glfw/src/init.c",
            "./vendor/glfw/src/input.c",
            "./vendor/glfw/src/monitor.c",
            "./vendor/glfw/src/null_init.c",
            "./vendor/glfw/src/null_joystick.c",
            "./vendor/glfw/src/null_monitor.c",
            "./vendor/glfw/src/null_window.c",
            "./vendor/glfw/src/platform.c",
            "./vendor/glfw/src/vulkan.c",
            "./vendor/glfw/src/window.c",
            "./vendor/glfw/src/x11_init.c",
            "./vendor/glfw/src/x11_monitor.c",
            "./vendor/glfw/src/x11_window.c",
            "./vendor/glfw/src/xkb_unicode.c",
            "./vendor/glfw/src/posix_module.c",
            "./vendor/glfw/src/posix_time.c",
            "./vendor/glfw/src/posix_thread.c",
            "./vendor/glfw/src/posix_module.c",
            "./vendor/glfw/src/posix_poll.c",
            "./vendor/glfw/src/glx_context.c",
            "./vendor/glfw/src/egl_context.c",
            "./vendor/glfw/src/osmesa_context.c",
            "./vendor/glfw/src/linux_joystick.c",
        ],
        extra_defines=[
            ("_GLFW_X11",),
            ("_POSIX_C_SOURCE", "200809L"),
        ],
        include_directories=[],
        depends_on=[]
    )


@buildspec
def glfw_windows_gnu_clang() -> None:
    StaticLibrary(
        name="glfw",
        out_filepath="./libraries/",
        sources=[
            "./vendor/glfw/src/context.c",
            "./vendor/glfw/src/init.c",
            "./vendor/glfw/src/input.c",
            "./vendor/glfw/src/monitor.c",
            "./vendor/glfw/src/null_init.c",
            "./vendor/glfw/src/null_joystick.c",
            "./vendor/glfw/src/null_monitor.c",
            "./vendor/glfw/src/null_window.c",
            "./vendor/glfw/src/platform.c",
            "./vendor/glfw/src/vulkan.c",
            "./vendor/glfw/src/window.c",
            "./vendor/glfw/src/win32_init.c",
            "./vendor/glfw/src/win32_joystick.c",
            "./vendor/glfw/src/win32_module.c",
            "./vendor/glfw/src/win32_monitor.c",
            "./vendor/glfw/src/win32_time.c",
            "./vendor/glfw/src/win32_thread.c",
            "./vendor/glfw/src/win32_window.c",
            "./vendor/glfw/src/wgl_context.c",
            "./vendor/glfw/src/egl_context.c",
            "./vendor/glfw/src/osmesa_context.c"
        ],
        extra_defines=[
            ("_GLFW_WIN32",),
            ("_CRT_SECURE_NO_WARNINGS",),
        ],
        include_directories=[],
        depends_on=[]
    )


@buildspec
def glad_windows_posix_gnu_clang() -> None:
    StaticLibrary(
        name="glad",
        out_filepath="./libraries/",
        sources=[
            "./vendor/glad/src/gl.c",
            "./vendor/glad/src/vulkan.c",
        ],
        include_directories=[],
    )


@buildspec
def stb_windows_posix_gnu_clang() -> None:
    StaticLibrary(
        name="stb",
        out_filepath="./libraries/",
        sources=[
            "./vendor/stb/src/stb_image.c"
        ],
        include_directories=[],
    )


@buildspec
def imgui_windows_posix_gnu_clang() -> None:
    StaticLibrary(
        name="ImGui",
        out_filepath="./libraries/",
        sources=[
            "./vendor/imgui/imgui_demo.cpp",
            "./vendor/imgui/imgui_draw.cpp",
            "./Car/src/ext/ImGui/imgui_impl_car.cpp",
            "./vendor/imgui/backends/imgui_impl_glfw.cpp",
            "./vendor/imgui/backends/imgui_impl_opengl3.cpp",
            "./vendor/imgui/imgui_tables.cpp",
            "./vendor/imgui/imgui_widgets.cpp",
            "./vendor/imgui/imgui.cpp",
        ],
        extra_defines=[
            ("GLFW_INCLUDE_NONE",),
        ],
        include_directories=[],
        depends_on=[
            "glad"
        ]
    )


@buildspec
def car_engine_windows_posix_gnu_clang() -> None:
    StaticLibrary(
        name="Car",
        out_filepath="./libraries/",
        sources=[
            "./Car/src/EntryPoint.cpp",
            "./Car/src/Utils.cpp",
            "./Car/src/Application.cpp",
            "./Car/src/Layers/Layer.cpp",
            "./Car/src/Layers/ImGuiLayer.cpp",
            "./Car/src/Layers/LayerStack.cpp",
            "./Car/src/Renderer/Buffer.cpp",
            "./Car/src/Renderer/Shader.cpp",
            "./Car/src/Renderer/Renderer2D.cpp",
            "./Car/src/internal/GLFW/Window.cpp",
            "./Car/src/internal/GLFW/Input.cpp",
            "./Car/src/internal/GLFW/Time.cpp",
            "./Car/src/internal/OpenGL/Renderer.cpp",
            "./Car/src/internal/OpenGL/GraphicsContext.cpp",
            "./Car/src/internal/OpenGL/Shader.cpp",
            "./Car/src/internal/OpenGL/IndexBuffer.cpp",
            "./Car/src/internal/OpenGL/VertexBuffer.cpp",
            "./Car/src/internal/OpenGL/SSBO.cpp",
            "./Car/src/internal/OpenGL/VertexArray.cpp",
            "./Car/src/internal/OpenGL/UniformBuffer.cpp",
            "./Car/src/internal/OpenGL/Texture2D.cpp",
        ],
        extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
        extra_defines=[
            ("CR_OPENGL",),
            ("GLFW_INCLUDE_NONE",),
        ],
        depends_on=[
            "ImGui", "glad", "stb", "glfw"
        ],
        include_directories=[],
    )


@buildspec(BuildSpecFlags.CORE | BuildSpecFlags.POSIX | BuildSpecFlags.WINDOWS, __name__ == "__main__")
def core_win_posix() -> None:
    Compiler.set_toolchain(Toolchain.CLANG)
    Compiler.set_c_standard(17)
    Compiler.set_cxx_standard(17)
    # Compiler.add_build_flags("-O3")
    Compiler.add_build_flags("-g")

    Compiler.add_define("DEBUG")
    Compiler.add_define("_DEBUG", "1")
    Compiler.add_define("CR_DEBUG")

    Compiler.add_include_directory("./Car/include/")
    Compiler.add_include_directory("./vendor/stb/include/")
    Compiler.add_include_directory("./vendor/glad/include/")
    Compiler.add_include_directory("./vendor/glfw/include/")
    Compiler.add_include_directory("./vendor/imgui/")
    Compiler.add_include_directory("./vendor/glm/")
    Compiler.add_include_directory("./vendor/debugbreak/")
    Compiler.add_include_directory("./vendor/entt/src/")

    Executable(
        name="sandbox.out",
        sources=[
            "SandBox/src/main.cpp",
        ],
        # TODO: Implement this system
        static_libraries=[
            "Car",
        ],
        extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
        extra_link_flags=[],
        include_directories=[],
        libraries=[
            "GL",
            "fmt",
        ],
        extra_defines=[
            ("GLFW_INCLUDE_NONE",),
        ]
    )


@unknown_argument
def unknown_arg(arg: str) -> bool:
    if arg == "--deps":
        print("getting dependencies")

        returncode = subprocess.run(["git", "submodule", "init"]).returncode
        if returncode != 0:
            print("failed to initialize git submodules")
            exit(1)
        returncode = subprocess.run(["git", "submodule", "update"]).returncode
        if returncode != 0:
            print("failed to update git submodules")
            exit(1)
        return True

    return False


if __name__ == "__main__":
    handle_argv(sys.argv[1:])
    build()
