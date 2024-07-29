#!/usr/bin/env python

import BuildIt
from BuildIt import buildspec, BuildSpecFlags, ExecResult
from pathlib import Path


OPENGL = 1
VULKAN = 2
rendering_api = OPENGL


compile_shaderc: bool = False


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
            "./vendor/glad/src/gl.c",
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
    if not compile_shaderc:
        shaderc = BuildIt.StaticLibrary(
            name="shaderc",
            out_filepath="./libraries/",
            sources=[
            ],
            depends_on=[],
            extra_build_flags=[],
            extra_defines=[
                ("ENABLE_HLSL",),
            ],
            include_directories=[],
        )

        return
    BuildIt.StaticLibrary(
        name="shaderc",
        out_filepath="./libraries/",
        sources=[
            "./vendor/shaderc/libshaderc/src/shaderc.cc",
            "./vendor/shaderc/libshaderc_util/src/compiler.cc",
            "./vendor/shaderc/libshaderc_util/src/message.cc",
            "./vendor/shaderc/libshaderc_util/src/resources.cc",
            "./vendor/shaderc/libshaderc_util/src/shader_stage.cc",
            "./vendor/shaderc/libshaderc_util/src/spirv_tools_wrapper.cc",
            "./vendor/shaderc/libshaderc_util/src/version_profile.cc",
            "./vendor/shaderc/third_party/glslang/SPIRV/CInterface/spirv_c_interface.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/GlslangToSpv.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/InReadableOrder.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/Logger.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/SPVRemapper.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/SpvBuilder.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/SpvPostProcess.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/SpvTools.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/disassemble.cpp",
            "./vendor/shaderc/third_party/glslang/SPIRV/doc.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/GenericCodeGen/CodeGen.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/GenericCodeGen/Link.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/HLSL/hlslAttributes.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/HLSL/hlslGrammar.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/HLSL/hlslOpMap.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/HLSL/hlslParseHelper.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/HLSL/hlslParseables.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/HLSL/hlslScanContext.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/HLSL/hlslTokenStream.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/Constant.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/InfoSink.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/Initialize.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/IntermTraverse.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/Intermediate.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/ParseContextBase.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/ParseHelper.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/PoolAlloc.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/RemoveTree.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/Scan.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/ShaderLang.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/SpirvIntrinsics.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/SymbolTable.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/Versions.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/attribute.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/glslang_tab.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/intermOut.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/iomapper.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/limits.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/linkValidate.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/parseConst.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/Pp.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/PpAtom.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/PpContext.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/PpScanner.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/PpTokens.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/propagateNoContraction.cpp",
            "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/reflection.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/assembly_grammar.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/binary.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/diagnostic.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/disassemble.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/enum_string_mapping.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/ext_inst.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/extensions.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/libspirv.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/name_mapper.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opcode.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/operand.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/aggressive_dead_code_elim_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/amd_ext_to_khr.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/analyze_live_input_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/basic_block.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/block_merge_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/block_merge_util.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/build_module.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/ccp_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/cfg.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/cfg_cleanup_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/code_sink.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/combine_access_chains.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/compact_ids_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/composite.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/const_folding_rules.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/constants.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/control_dependence.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/convert_to_half_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/convert_to_sampled_image_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/copy_prop_arrays.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/dataflow.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/dead_branch_elim_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/dead_insert_elim_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/dead_variable_elimination.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/debug_info_manager.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/decoration_manager.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/def_use_manager.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/desc_sroa.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/desc_sroa_util.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/dominator_analysis.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/dominator_tree.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_constant_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_functions_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_functions_util.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_io_components_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_members_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_output_stores_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/feature_manager.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/fix_func_call_arguments.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/fix_storage_class.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/flatten_decoration_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/fold.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/fold_spec_constant_op_and_composite_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/folding_rules.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/freeze_spec_constant_value_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/function.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/graphics_robust_access_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/if_conversion.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/inline_exhaustive_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/inline_opaque_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/inline_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/inst_debug_printf_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/instruction.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/instruction_list.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/instrument_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/interface_var_sroa.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/interp_fixup_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/invocation_interlock_placement_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/ir_context.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/ir_loader.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/licm_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/liveness.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/local_access_chain_convert_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/local_redundancy_elimination.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/local_single_block_elim_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/local_single_store_elim_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_dependence.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_dependence_helpers.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_descriptor.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_fission.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_fusion.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_fusion_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_peeling.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_unroller.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_unswitch_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/loop_utils.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/mem_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/merge_return_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/modify_maximal_reconvergence.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/module.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/opextinst_forward_ref_fixup_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/optimizer.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/pass_manager.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/private_to_local_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/propagator.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/reduce_load_size.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/redundancy_elimination.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/register_pressure.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/relax_float_ops_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/remove_dontinline_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/remove_duplicates_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/remove_unused_interface_variables_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/replace_desc_array_access_using_var_index.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/replace_invalid_opc.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/scalar_analysis.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/scalar_analysis_simplification.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/scalar_replacement_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/set_spec_constant_default_value_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/simplification_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/spread_volatile_semantics.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/ssa_rewrite_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/strength_reduction_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/strip_debug_info_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/strip_nonsemantic_info_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/struct_cfg_analysis.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/switch_descriptorset_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/trim_capabilities_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/type_manager.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/types.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/unify_const_pass.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/upgrade_memory_model.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/value_number_table.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/vector_dce.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/workaround1209.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/opt/wrap_opkill.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/parsed_operand.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/print.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/software_version.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/spirv_endian.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/spirv_fuzzer_options.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/spirv_optimizer_options.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/spirv_reducer_options.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/spirv_target_env.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/spirv_validator_options.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/table.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/text.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/text_handler.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/util/bit_vector.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/util/parse_number.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/util/string_utils.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/basic_block.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/construct.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/function.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/instruction.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_adjacency.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_annotation.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_arithmetics.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_atomics.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_barriers.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_bitwise.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_builtins.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_capability.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_cfg.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_composites.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_constants.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_conversion.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_debug.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_decorations.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_derivatives.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_execution_limitations.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_extensions.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_function.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_id.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_image.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_instruction.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_interfaces.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_layout.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_literals.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_logicals.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_memory.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_memory_semantics.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_mesh_shading.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_misc.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_mode_setting.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_non_uniform.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_primitives.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_ray_query.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_ray_tracing.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_ray_tracing_reorder.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_scopes.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_small_type_uses.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validate_type.cpp",
            "./vendor/shaderc/third_party/spirv-tools/source/val/validation_state.cpp",
        ],
        extra_build_flags=[
        ],
        extra_defines=[
            ("ENABLE_HLSL",),
        ],
        depends_on=[
        ],
        include_directories=[],
    ).attach_precompiled_headers(
        "./vendor/shaderc/third_party/spirv-tools/source/pch_source.h",
        "./vendor/shaderc/third_party/spirv-tools/source/opt/pch_source_opt.h",
        "./vendor/shaderc/third_party/spirv-tools/source/reduce/pch_source_reduce.h",
        "./vendor/shaderc/third_party/glslang/glslang/HLSL/pch.h",
        "./vendor/shaderc/third_party/glslang/glslang/MachineIndependent/pch.h",
    ).force_language_cxx()
    BuildIt.add_include_directory("./vendor/shaderc/third_party/spirv-tools/")
    BuildIt.add_include_directory("./vendor/shaderc/libshaderc_util/include/")
    BuildIt.add_include_directory("./vendor/shaderc/third_party/spirv-headers/include/")
    BuildIt.add_include_directory("./vendor/shaderc/third_party/spirv-tools/include/")
    BuildIt.add_include_directory("./vendor/shaderc/third_party/glslang/")
    BuildIt.add_include_directory("./vendor/shaderc/third_party/spirv-headers/include/binary")


@buildspec
def car_engine_windows_posix_gnu_clang() -> None:
    carlib = BuildIt.StaticLibrary(
        name="Car",
        out_filepath="./libraries/",
        sources=[
            "./Car/src/EntryPoint.cpp",
            "./Car/src/Utils.cpp",
            "./Car/src/Application.cpp",
            "./Car/src/ResourceManager.cpp",
            "./Car/src/Time.cpp",
            "./Car/src/Input.cpp",
            "./Car/src/Window.cpp",
            "./Car/src/Layers/Layer.cpp",
            "./Car/src/Layers/ImGuiLayer.cpp",
            "./Car/src/Layers/LayerStack.cpp",
            "./Car/src/Renderer/Buffer.cpp",
            "./Car/src/Renderer/Shader.cpp",
            "./Car/src/Renderer/Renderer2D.cpp",
            "./Car/src/Renderer/Font.cpp",
            "./Car/src/Renderer/GraphicsContext.cpp",
        ],
        extra_build_flags=["-Wall", "-Wextra", "-Werror", "-pedantic"],
        extra_defines=[
            ("GLFW_INCLUDE_NONE",),
        ],
        depends_on=[
            "ImGui", "glad", "stb", "glfw", "freetype", "shaderc"
        ],
        include_directories=[],
    )
    if rendering_api == OPENGL:
        carlib.add_sources(
            "./Car/src/internal/OpenGL/Renderer.cpp",
            "./Car/src/internal/OpenGL/GraphicsContext.cpp",
            "./Car/src/internal/OpenGL/Shader.cpp",
            "./Car/src/internal/OpenGL/IndexBuffer.cpp",
            "./Car/src/internal/OpenGL/VertexBuffer.cpp",
            "./Car/src/internal/OpenGL/SSBO.cpp",
            "./Car/src/internal/OpenGL/VertexArray.cpp",
            "./Car/src/internal/OpenGL/UniformBuffer.cpp",
            "./Car/src/internal/OpenGL/Texture2D.cpp",
        )
        carlib.add_define("CR_OPENGL")
    elif rendering_api == VULKAN:
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


@BuildIt.unknown_argument
def unknown_arg(arg: str) -> bool:
    global rendering_api
    
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
        print("    --shaderc compiles the shaderc library into a staticly linked library")
        print("    --opengl, -ogl to use OpenGL rendering backend (default)")
        print("    --vulkan, -vk to use Vulkan rendering backend")
        print("    --format formats the code (required clang-format)")
    elif arg == "--format":
        files = list(str(path) for path in Path("./Car").glob("**/*.cpp"))
        files += list(str(path) for path in Path("./Car").glob("**/*.hpp"))
        files += list(str(path) for path in Path("./SandBox").glob("**/*.hpp"))
        files += list(str(path) for path in Path("./SandBox").glob("**/*.cpp"))
        files += list(str(path) for path in Path("./examples").glob("**/*.hpp"))
        files += list(str(path) for path in Path("./examples").glob("**/*.cpp"))
        
        exit(BuildIt.exec_cmd("clang-format", "-i", *files, "--verbose").returncode)
    elif arg == "--shaderc":
        global compile_shaderc
        compile_shaderc = True
        return True
    elif arg == "-ogl" or arg == "--opengl":
        rendering_api = OPENGL
        return True
    elif arg == "-vk" or arg == "--vulkan":
        rendering_api = VULKAN
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


def compile_shaders() -> None:
    if BuildIt.exec_cmd("glslc", "-fshader-stage=frag", "resources/shaders/temp/glsl/fragmeant.glsl", "-o", "resources/shaders/temp/spv/fragmeant.spv").returncode:
            exit(1)
    if BuildIt.exec_cmd("glslc", "-fshader-stage=vert", "resources/shaders/temp/glsl/vertex.glsl", "-o", "resources/shaders/temp/spv/vertex.spv").returncode:
            exit(1)



if __name__ == "__main__":
    compile_shaders()
    BuildIt.handle_argv()
    BuildIt.build()
