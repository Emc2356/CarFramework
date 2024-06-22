#!/usr/bin/env python

from BuildIt import *
import sys


@define
def glfw_posix():
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
        ],
        include_directories=[
            "./vendor/glfw/include/",
        ],
        depends_on=[]
    )


@define
def glfw_windows() -> None:
    StaticLibrary(
        name="glfw",
        out_filepath="./libraries/",
        sources=[
            "src/glfw_config.h",
            "src/context.c",
            "src/init.c",
            "src/input.c",
            "src/monitor.c",
            "src/null_init.c",
            "src/null_joystick.c",
            "src/null_monitor.c",
            "src/null_window.c",
            "src/platform.c",
            "src/vulkan.c",
            "src/window.c",
            "src/win32_init.c",
            "src/win32_joystick.c",
            "src/win32_module.c",
            "src/win32_monitor.c",
            "src/win32_time.c",
            "src/win32_thread.c",
            "src/win32_window.c",
            "src/wgl_context.c",
            "src/egl_context.c",
            "src/osmesa_context.c"
        ],
        extra_defines=[
			("_GLFW_WIN32",),
			("_CRT_SECURE_NO_WARNINGS",),
        ],
        include_directories=[
            "./vendor/glfw/include/",
        ],
        depends_on=[]
    )


@define
def define_details() -> None:
    Compiler.set_toolchain("GNU")
    Compiler.set_c_standard(17)
    Compiler.set_cxx_standard(17)
    Compiler.add_build_flags("-g3", "-Og")

    Compiler.add_define("DEBUG")
    Compiler.add_define("_DEBUG", "1")
    Compiler.add_define("CR_DEBUG")

    PreCompiledHeader(
        source="./Car/include/Car/Core/crpch.hpp",
        extra_build_flags=[],
        extra_defines=[]
    )

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
        include_directories=[
            "./Car/include/",
            "./vendor/imgui/",
            "./vendor/glad/include/",
            "./vendor/glfw/include/",
            "./vendor/debugbreak/",
        ],
        depends_on=[
            "glad"
        ]
    )
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
    StaticLibrary(
        name="stb",
        out_filepath="./libraries/",
        sources=[
            "./vendor/stb/src/stb_image.c"
        ],
        include_directories=[
            "./vendor/stb/include/",
        ],
    )

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
            "./Car/src/internal/GLFW/Window.cpp",
            "./Car/src/internal/GLFW/Input.cpp",
            "./Car/src/internal/GLFW/Time.cpp",
            "./Car/src/internal/OpenGL/GraphicsContext.cpp",
            "./Car/src/internal/OpenGL/Shader.cpp",
            "./Car/src/internal/OpenGL/IndexBuffer.cpp",
            "./Car/src/internal/OpenGL/VertexBuffer.cpp",
            "./Car/src/internal/OpenGL/VertexArray.cpp",
            "./Car/src/internal/OpenGL/UniformBuffer.cpp",
            "./Car/src/internal/OpenGL/Texture2D.cpp",
            "./Car/src/internal/OpenGL/Renderer.cpp",
        ],
        extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
        extra_defines=[
            ("CR_OPENGL",),
            ("GLFW_INCLUDE_NONE",),
        ],
        depends_on=[
            "ImGui", "glad", "stb", "glfw"
        ],
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
    )

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


if __name__ == "__main__":
    handle_argv(sys.argv[1:])
    build()
