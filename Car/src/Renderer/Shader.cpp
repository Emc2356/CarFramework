#include "Car/Renderer/Shader.hpp"

#include "Car/Utils.hpp"

#include <shaderc/env.h>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <stdexcept>


// CR_FORCE_INLINE std::string compileSingleShader(const std::string& path, const shaderc_shader_kind kind) {
//     std::string sourceCode = Car::readFile(path);
//     shaderc::CompileOptions options;
//     shaderc::Compiler compiler;
//     options.SetOptimizationLevel(shaderc_optimization_level_performance);
//     options.SetSourceLanguage(shaderc_source_language_glsl);
//     options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

//     shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(sourceCode, kind, path.c_str(), options);
//     if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
//         throw std::runtime_error(result.GetErrorMessage());
//     }
    
//     uint32_t count = result.cend() - result.cbegin();
//     std::string ret;
//     ret.resize(count*4);
    
//     for (uint32_t i = 0; i < count; i++) {
//         uint32_t value = result.cbegin()[i];
//         ret[i*4 + 0] = value & 0xFF000000;
//         ret[i*4 + 1] = value & 0x00FF0000;
//         ret[i*4 + 2] = value & 0x0000FF00;
//         ret[i*4 + 3] = value & 0x000000FF;
//     }

//     return ret;
// }


namespace Car {
    Ref<Shader> Shader::Create(const std::string& vertexShaderName, const std::string& fragmeantShaderName) {
        return Shader::CreateImpl(
            readFile("./resources/shaders/" + vertexShaderName),
            readFile("./resources/shaders/" + fragmeantShaderName)
        );;
    }
}
