#!/usr/bin/env python

import BuildIt
from BuildIt import buildspec, BuildSpecFlags, ExecResult
from pathlib import Path


build_examples = False

build_shaderc = False


@buildspec
def pch_windows_posix_gnu_clang() -> None:
    BuildIt.PreCompiledHeader(
        source="./Car/include/Car/Core/crpch.hpp",
        extra_build_flags=[],
        extra_defines=[]
    )


@buildspec
def glfw_posix_gnu_clang() -> None:
    BuildIt.StaticLibrary(
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
    BuildIt.add_include_directory("./vendor/glfw/include")


@buildspec
def glfw_windows_gnu_clang() -> None:
    BuildIt.StaticLibrary(
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
    BuildIt.add_include_directory("./vendor/glfw/include")


@buildspec
def glad_windows_posix_gnu_clang() -> None:
    BuildIt.StaticLibrary(
        name="glad",
        out_filepath="./libraries/",
        sources=[
            "./vendor/glad/src/vulkan.c",
        ],
        include_directories=[],
    )
    BuildIt.add_include_directory("./vendor/glad/include")


@buildspec
def stb_windows_posix_gnu_clang() -> None:
    BuildIt.StaticLibrary(
        name="stb",
        out_filepath="./libraries/",
        sources=[
            "./vendor/stb/src/stb_image.c"
        ],
        include_directories=[],
    )
    BuildIt.add_include_directory("./vendor/stb/include")


@buildspec
def imgui_windows_posix_gnu_clang() -> None:
    BuildIt.StaticLibrary(
        name="ImGui",
        out_filepath="./libraries/",
        sources=[
            "./vendor/imgui/imgui_demo.cpp",
            "./vendor/imgui/imgui_draw.cpp",
            "./Car/src/ext/ImGui/imgui_impl_car.cpp",
            "./vendor/imgui/backends/imgui_impl_glfw.cpp",
            "./vendor/imgui/backends/imgui_impl_opengl3.cpp",
            "./vendor/imgui/backends/imgui_impl_vulkan.cpp",
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
    BuildIt.add_include_directory("./vendor/imgui")


@buildspec
def freetype_windows_posix_gnu_clang() -> None:
    # https://github.com/freetype/freetype/blob/master/docs/INSTALL.ANY
    BuildIt.StaticLibrary(
        name="freetype",
        out_filepath="./libraries/",
        sources=[
            "./vendor/freetype/src/base/ftbase.c",
            "./vendor/freetype/src/base/ftinit.c",
            "./vendor/freetype/src/base/ftsystem.c",
            "./vendor/freetype/src/base/ftdebug.c",
            "./vendor/freetype/src/base/ftbbox.c",
            "./vendor/freetype/src/base/ftglyph.c",
            "./vendor/freetype/src/base/ftbdf.c",
            "./vendor/freetype/src/base/ftbitmap.c",
            "./vendor/freetype/src/base/ftcid.c",
            "./vendor/freetype/src/base/ftfstype.c",
            "./vendor/freetype/src/base/ftgasp.c",
            "./vendor/freetype/src/base/ftgxval.c",
            "./vendor/freetype/src/base/ftmm.c",
            "./vendor/freetype/src/base/ftotval.c",
            "./vendor/freetype/src/base/ftpatent.c",
            "./vendor/freetype/src/base/ftpfr.c",
            "./vendor/freetype/src/base/ftstroke.c",
            "./vendor/freetype/src/base/ftsynth.c",
            "./vendor/freetype/src/base/fttype1.c",
            "./vendor/freetype/src/base/ftwinfnt.c",
            "./vendor/freetype/src/base/ftmac.c",
            "./vendor/freetype/src/bdf/bdf.c",
            "./vendor/freetype/src/cff/cff.c",
            "./vendor/freetype/src/cid/type1cid.c",
            "./vendor/freetype/src/pcf/pcf.c",
            "./vendor/freetype/src/pfr/pfr.c",
            "./vendor/freetype/src/sfnt/sfnt.c",
            "./vendor/freetype/src/truetype/truetype.c",
            "./vendor/freetype/src/type1/type1.c",
            "./vendor/freetype/src/type42/type42.c",
            "./vendor/freetype/src/winfonts/winfnt.c",
            "./vendor/freetype/src/smooth/smooth.c",
            "./vendor/freetype/src/raster/raster.c",
            "./vendor/freetype/src/sdf/sdf.c",
            "./vendor/freetype/src/autofit/autofit.c",
            "./vendor/freetype/src/cache/ftcache.c",
            "./vendor/freetype/src/gzip/ftgzip.c",
            "./vendor/freetype/src/lzw/ftlzw.c",
            "./vendor/freetype/src/bzip2/ftbzip2.c",
            "./vendor/freetype/src/gxvalid/gxvalid.c",
            "./vendor/freetype/src/otvalid/otvalid.c",
            "./vendor/freetype/src/psaux/psaux.c",
            "./vendor/freetype/src/pshinter/pshinter.c",
            "./vendor/freetype/src/psnames/psnames.c",
            "./vendor/freetype/src/svg/ftsvg.c",
        ],
        extra_build_flags=[],
        extra_defines=[
            ("FT2_BUILD_LIBRARY",)
        ],
        depends_on=[],
        include_directories=[],
    )
    BuildIt.add_include_directory("./vendor/freetype/include")


@buildspec
def shaderc_windows_posix_gnu_clang() -> None:
    # this are the pre compiled headers that it uses but since i am doing 
    # unity builds i dont think they will make any major difference 
    # "./vendor/shaderc/third_party/spirv-tools/source/pch_source.h",
    # "./vendor/shaderc/third_party/spirv-tools/source/opt/pch_source_opt.h",
    # "./vendor/shaderc/third_party/spirv-tools/source/reduce/pch_source_reduce.h",
    # "./vendor/shaderc/third_party/glslang/glslang/HLSL/pch.h",
    # "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/pch.h",
    if not build_shaderc:
        BuildIt.StaticLibrary(
            name="shaderc",
            out_filepath="./libraries/",
            sources=[],
            depends_on=[],
            extra_build_flags=[],
            extra_defines=[],
            include_directories=[]
        )
        return
    BuildIt.StaticLibrary(
        name="shaderc",
        out_filepath="./libraries/",
        sources=[
            "./vendor/unity_builds/shaderc.cpp",
            "./vendor/unity_builds/glslang1.cpp",
            "./vendor/unity_builds/glslang2.cpp",
            "./vendor/unity_builds/spv_tools1.cpp",
            "./vendor/unity_builds/spv_tools2.cpp",
            "./vendor/unity_builds/spv_tools3.cpp",
            "./vendor/unity_builds/spv_tools4.cpp",
            "./vendor/unity_builds/spv_tools5.cpp",
            "./vendor/unity_builds/spv_tools6.cpp",
            "./vendor/unity_builds/spv_tools7.cpp",
            "./vendor/unity_builds/spv_tools8.cpp",
        ],
        depends_on=[],
        extra_build_flags=[],
        extra_defines=[
            ("ENABLE_HLSL",),
        ],
        include_directories=[
            "./"
        ]
    ).force_language_cxx()
    
    BuildIt.add_include_directory("./vendor/shaderc/third_party/spirv-tools/")
    BuildIt.add_include_directory("./vendor/shaderc/libshaderc_util/include/")
    BuildIt.add_include_directory("./vendor/shaderc/third_party/spirv-headers/include/")
    BuildIt.add_include_directory("./vendor/shaderc/third_party/spirv-tools/include/")
    BuildIt.add_include_directory("./vendor/shaderc/third_party/glslang/")
    BuildIt.add_include_directory("./vendor/shaderc/third_party/spirv-headers/include/binary")


@buildspec
def spirv_cross_gnu_clang_posix() -> None:
    BuildIt.StaticLibrary(
        name="spirv_cross",
        out_filepath="./libraries/",
        sources=[
            "./vendor/unity_builds/spirv_cross.cpp"
        ],
        extra_defines=[
            
        ],
        include_directories=["./"],
        depends_on=[]
    )
    BuildIt.add_include_directory("vendor/spirv_cross")


@buildspec
def car_engine_windows_posix_gnu_clang() -> None:
    carlib = BuildIt.StaticLibrary(
        name="Car",
        out_filepath="./libraries/",
        sources=[
            "./Car/src/Utils.cpp",
            "./Car/src/Application.cpp",
            "./Car/src/ResourceManager.cpp",
            "./Car/src/Time.cpp",
            "./Car/src/Input.cpp",
            "./Car/src/Window.cpp",
            "./Car/src/Random.cpp",
            "./Car/src/Layers/Layer.cpp",
            "./Car/src/Layers/ImGuiLayer.cpp",
            "./Car/src/Layers/LayerStack.cpp",
            "./Car/src/Renderer/Buffer.cpp",
            "./Car/src/Renderer/Renderer2D.cpp",
            "./Car/src/Renderer/Font.cpp",
            "./Car/src/Renderer/GraphicsContext.cpp",
        ],
        extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
        extra_defines=[
            ("GLFW_INCLUDE_NONE",),
        ],
        depends_on=[
            "ImGui", "glad", "stb", "glfw", "freetype", "shaderc", "spirv_cross"
        ],
        include_directories=[],
    )
    carlib.add_sources(
        "./Car/src/internal/Vulkan/Renderer.cpp",
        "./Car/src/internal/Vulkan/GraphicsContext.cpp",
        "./Car/src/internal/Vulkan/Shader.cpp",
        "./Car/src/internal/Vulkan/IndexBuffer.cpp",
        "./Car/src/internal/Vulkan/VertexBuffer.cpp",
        "./Car/src/internal/Vulkan/SSBO.cpp",
        "./Car/src/internal/Vulkan/VertexArray.cpp",
        "./Car/src/internal/Vulkan/UniformBuffer.cpp",
        "./Car/src/internal/Vulkan/Texture2D.cpp",
    )
    carlib.add_define("CR_VULKAN")
    BuildIt.add_include_directory("./Car/include/")


@buildspec(BuildSpecFlags.CORE | BuildSpecFlags.POSIX | BuildSpecFlags.WINDOWS, __name__ == "__main__")
def core_win_posix() -> None:
    BuildIt.set_toolchain(BuildIt.Toolchain.CLANG)
    BuildIt.set_c_standard(99)
    BuildIt.set_cxx_standard(17)
    # BuildIt.rebuild_if_includes_changed()

    if not BuildIt.is_release():
        BuildIt.add_define("CR_DEBUG")
        BuildIt.add_define("CR_HAVE_SHADERC")

    BuildIt.add_include_directory("./vendor/glm/")
    BuildIt.add_include_directory("./vendor/debugbreak/")
    
    BuildIt.Executable(
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
            "fmt"
        ],
        extra_defines=[
            ("GLFW_INCLUDE_NONE",),
        ]
    )
    
    if build_examples:
        BuildIt.Executable(
            name="raycasting.out",
            sources=[
                "examples/RayCasting.cpp"
            ],
            static_libraries=[
                "Car"
            ],
            extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
            extra_link_flags=[],
            include_directories=[],
            libraries=["fmt"],
            extra_defines=[
                ("GLFW_INCLUDE_NONE",),
            ]
        )
        
        BuildIt.Executable(
            name="Maze.out",
            sources=[
                "examples/Maze.cpp"
            ],
            static_libraries=[
                "Car"
            ],
            extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
            extra_link_flags=[],
            include_directories=[],
            libraries=["fmt"],
            extra_defines=[
                ("GLFW_INCLUDE_NONE",),
            ]
        )


@BuildIt.unknown_argument
def unknown_arg(arg: str) -> bool:    
    SPIRV_TOOLS_DIR = "./vendor/shaderc/third_party/spirv-tools"
    GRAMMAR_PROCESSING_SCRIPT = f"{SPIRV_TOOLS_DIR}/utils/generate_grammar_tables.py"
    XML_REGISTRY_PROCESSING_SCRIPT = f"{SPIRV_TOOLS_DIR}/utils/generate_registry_tables.py"
    LANG_HEADER_PROCESSING_SCRIPT = f"{SPIRV_TOOLS_DIR}/utils/generate_language_headers.py"
    SPIRV_TOOLS_BUILD_VERSION_INC_GENERATOR = f"{SPIRV_TOOLS_DIR}/utils/update_build_version.py"

    SPIRV_HEADER_INCLUDE_DIR = "./vendor/shaderc/third_party/spirv-headers/include"
    DEBUGINFO_GRAMMAR_JSON_FILE = f"{SPIRV_HEADER_INCLUDE_DIR}/spirv/unified1/extinst.debuginfo.grammar.json"
    CLDEBUGINFO100_GRAMMAR_JSON_FILE = f"{SPIRV_HEADER_INCLUDE_DIR}/spirv/unified1/extinst.opencl.debuginfo.100.grammar.json"
    VKDEBUGINFO100_GRAMMAR_JSON_FILE = f"{SPIRV_HEADER_INCLUDE_DIR}/spirv/unified1/extinst.nonsemantic.shader.debuginfo.100.grammar.json"

    CONFIG_VERSION = "unified1"
    SPIRV_TOOLS_BINARY_DIR = f"{SPIRV_HEADER_INCLUDE_DIR}/binary"
    GRAMMAR_JSON_FILE = f"{SPIRV_HEADER_INCLUDE_DIR}/spirv/{CONFIG_VERSION}/spirv.core.grammar.json"
    GRAMMAR_INSTS_INC_FILE = f"{SPIRV_TOOLS_BINARY_DIR}/core.insts-{CONFIG_VERSION}.inc"
    GRAMMAR_KINDS_INC_FILE = f"{SPIRV_TOOLS_BINARY_DIR}/operand.kinds-{CONFIG_VERSION}.inc"

    GRAMMAR_EXTENSION_ENUM_INC_FILE = f"{SPIRV_TOOLS_BINARY_DIR}/extension_enum.inc"
    GRAMMAR_ENUM_STRING_MAPPING_INC_FILE = f"{SPIRV_TOOLS_BINARY_DIR}/enum_string_mapping.inc"

    if arg == "--help" or arg == "-h":
        print("Car build script:")
        print("    --deps fetchs all of the dependencies and it initializes them")
        print("    --format formats the code (requires clang-format)")
        print("    --shaderc build shaderc (required to compile shaders on the fly)")
        print("    --examples builds the examples")
    elif arg == "--format":
        files = list(str(path) for path in Path("./Car").glob("**/*.cpp"))
        files += list(str(path) for path in Path("./Car").glob("**/*.hpp"))
        files += list(str(path) for path in Path("./SandBox").glob("**/*.hpp"))
        files += list(str(path) for path in Path("./SandBox").glob("**/*.cpp"))
        files += list(str(path) for path in Path("./examples").glob("**/*.hpp"))
        files += list(str(path) for path in Path("./examples").glob("**/*.cpp"))
        
        exit(BuildIt.exec_cmd("clang-format", "-i", *files, "--verbose").returncode)
    elif arg == "--shaderc":
        global build_shaderc
        build_shaderc = True
        return True
    elif arg == "--examples":
        global build_examples
        build_examples = True
        return True
    elif arg == "--deps":
        if BuildIt.exec_cmd("git", "submodule", "init"):
            print("failed to initialize git submodules")
            exit(1)
        if BuildIt.exec_cmd("git", "submodule", "update"):
            print("failed to update git submodules")
            exit(1)

        if BuildIt.exec_cmd("utils/git-sync-deps", is_python=True, cwd="./vendor/shaderc"):
            print("failed to fetch shaderc deps")
            exit(1)
        if BuildIt.exec_cmd("utils/git-sync-deps", is_python=True, cwd="./vendor/shaderc/third_party/spirv-tools"):
            print("failed to fetch spirv-tools deps")
            exit(1)

        # spvtools_core_tables("unified1")
        if BuildIt.exec_cmd(
            GRAMMAR_PROCESSING_SCRIPT,
            f"--spirv-core-grammar={GRAMMAR_JSON_FILE}",
            f"--extinst-debuginfo-grammar={DEBUGINFO_GRAMMAR_JSON_FILE}",
            f"--extinst-cldebuginfo100-grammar={CLDEBUGINFO100_GRAMMAR_JSON_FILE}",
            f"--core-insts-output={GRAMMAR_INSTS_INC_FILE}",
            f"--operand-kinds-output={GRAMMAR_KINDS_INC_FILE}",
            "--output-language=c++",
            is_python=True
        ):
            print("failed to generate spirv-tools core tables")
            exit(1)

        # spvtools_enum_string_mapping("unified1")
        if BuildIt.exec_cmd(
            GRAMMAR_PROCESSING_SCRIPT,
            f"--spirv-core-grammar={GRAMMAR_JSON_FILE}",
            f"--extinst-debuginfo-grammar={DEBUGINFO_GRAMMAR_JSON_FILE}",
            f"--extinst-cldebuginfo100-grammar={CLDEBUGINFO100_GRAMMAR_JSON_FILE}",
            f"--extension-enum-output={GRAMMAR_EXTENSION_ENUM_INC_FILE}",
            f"--enum-string-mapping-output={GRAMMAR_ENUM_STRING_MAPPING_INC_FILE}",
            "--output-language=c++",
            is_python=True
        ):
            print("failed to generate enum string mapping for spirv-tools")
            exit(1)

        # spvtools_opencl_tables("unified1")
        if BuildIt.exec_cmd(
            GRAMMAR_PROCESSING_SCRIPT,
            f"--extinst-opencl-grammar={SPIRV_HEADER_INCLUDE_DIR}/spirv/{CONFIG_VERSION}/extinst.opencl.std.100.grammar.json",
            f"--opencl-insts-output={SPIRV_TOOLS_BINARY_DIR}/opencl.std.insts.inc",
            is_python=True
        ):
            print("failed to generate opencl table for spirv-tools")
            exit(1)

        # spvtools_glsl_tables("unified1")
        if BuildIt.exec_cmd(
            GRAMMAR_PROCESSING_SCRIPT,
            f"--extinst-glsl-grammar={SPIRV_HEADER_INCLUDE_DIR}/spirv/{CONFIG_VERSION}/extinst.glsl.std.450.grammar.json",
            f"--glsl-insts-output={SPIRV_TOOLS_BINARY_DIR}/glsl.std.450.insts.inc",
            "--output-language=c++",
            is_python=True
        ):
            print("failed to generate glsl table for spirv-tools")
            exit(1)

        def spvtools_vendor_tables(vendor_table: str, short_name: str, operand_kind_prefix: str) -> ExecResult:
            return BuildIt.exec_cmd(
                GRAMMAR_PROCESSING_SCRIPT,
                f"--extinst-vendor-grammar={SPIRV_HEADER_INCLUDE_DIR}/spirv/unified1/extinst.{vendor_table}.grammar.json",
                f"--vendor-insts-output={SPIRV_TOOLS_BINARY_DIR}/{vendor_table}.insts.inc",
                f"--vendor-operand-kind-prefix={operand_kind_prefix}",
                is_python=True
            )

        if spvtools_vendor_tables("spv-amd-shader-explicit-vertex-parameter", "spv-amd-sevp", ""):
            print("spvtools_vendor_tables(\"spv-amd-shader-explicit-vertex-parameter\", \"spv-amd-sevp\", \"\") failed")
            exit(1)
        if spvtools_vendor_tables("spv-amd-shader-trinary-minmax", "spv-amd-stm", ""):
            print("spvtools_vendor_tables(\"spv-amd-shader-trinary-minmax\", \"spv-amd-stm\", \"\") failed")
            exit(1)
        if spvtools_vendor_tables("spv-amd-gcn-shader", "spv-amd-gs", ""):
            print("spvtools_vendor_tables(\"spv-amd-gcn-shader\", \"spv-amd-gs\", \"\") failed")
            exit(1)
        if spvtools_vendor_tables("spv-amd-shader-ballot", "spv-amd-sb", ""):
            print("spvtools_vendor_tables(\"spv-amd-shader-ballot\", \"spv-amd-sb\", \"\") failed")
            exit(1)
        if spvtools_vendor_tables("debuginfo", "debuginfo", ""):
            print("spvtools_vendor_tables(\"debuginfo\", \"debuginfo\", \"\") failed")
            exit(1)
        if spvtools_vendor_tables("opencl.debuginfo.100", "cldi100", "CLDEBUG100_"):
            print("spvtools_vendor_tables(\"opencl.debuginfo.100\", \"cldi100\", \"CLDEBUG100_\") failed")
            exit(1)
        if spvtools_vendor_tables("nonsemantic.shader.debuginfo.100", "shdi100", "SHDEBUG100_"):
            print("spvtools_vendor_tables(\"nonsemantic.shader.debuginfo.100\", \"shdi100\", \"SHDEBUG100_\") failed")
            exit(1)
        if spvtools_vendor_tables("nonsemantic.clspvreflection", "clspvreflection", ""):
            print("spvtools_vendor_tables(\"nonsemantic.clspvreflection\", \"clspvreflection\", \"\") failed")
            exit(1)
        if spvtools_vendor_tables("nonsemantic.vkspreflection", "vkspreflection", ""):
            print("spvtools_vendor_tables(\"nonsemantic.vkspreflection\", \"vkspreflection\", \"\") failed")
            exit(1)


        # spvtools_extinst_lang_headers("DebugInfo" ${DEBUGINFO_GRAMMAR_JSON_FILE})
        if BuildIt.exec_cmd(
            LANG_HEADER_PROCESSING_SCRIPT,
            f"--extinst-grammar={DEBUGINFO_GRAMMAR_JSON_FILE}",
            f"--extinst-output-path={SPIRV_TOOLS_BINARY_DIR}/DebugInfo.h",
            is_python=True
        ):
            print("failed to generate DebugInfo.h for spirv-tools")
            exit(1)

        # spvtools_extinst_lang_headers("OpenCLDebugInfo100" ${CLDEBUGINFO100_GRAMMAR_JSON_FILE})
        if BuildIt.exec_cmd(
            LANG_HEADER_PROCESSING_SCRIPT,
            f"--extinst-grammar={CLDEBUGINFO100_GRAMMAR_JSON_FILE}",
            f"--extinst-output-path={SPIRV_TOOLS_BINARY_DIR}/OpenCLDebugInfo100.h",
            is_python=True
        ):
            print("failed to generate OpenCLDebugInfo100.h for spirv-tools")
            exit(1)
        # spvtools_extinst_lang_headers("NonSemanticShaderDebugInfo100" ${VKDEBUGINFO100_GRAMMAR_JSON_FILE})
        if BuildIt.exec_cmd(
            LANG_HEADER_PROCESSING_SCRIPT,
            f"--extinst-grammar={VKDEBUGINFO100_GRAMMAR_JSON_FILE}",
            f"--extinst-output-path={SPIRV_TOOLS_BINARY_DIR}/NonSemanticShaderDebugInfo100.h",
            is_python=True
        ):
            print("failed to generate NonSemanticShaderDebugInfo100.h for spirv-tools")
            exit(1)

        if BuildIt.exec_cmd(
            XML_REGISTRY_PROCESSING_SCRIPT,
            f"--xml={SPIRV_HEADER_INCLUDE_DIR}/spirv/spir-v.xml",
            f"--generator-output={SPIRV_TOOLS_BINARY_DIR}/generators.inc",
            is_python=True
        ):
            print("failed to generate generators.inc")
            exit(1)

        if BuildIt.exec_cmd(
            SPIRV_TOOLS_BUILD_VERSION_INC_GENERATOR,
            f"{SPIRV_TOOLS_DIR}/CHANGES",
            f"{SPIRV_TOOLS_BINARY_DIR}/build-version.inc",
            is_python=True
        ):
            print("failed to generate build-version.inc")
            exit(1)

        exit(0)

    return False


if __name__ == "__main__":
    BuildIt.handle_argv()
    BuildIt.build()
