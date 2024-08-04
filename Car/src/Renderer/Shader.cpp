#include "Car/Renderer/Shader.hpp"

#include "Car/Utils.hpp"
#include "Car/ResourceManager.hpp"

#ifdef CR_HAVE_SHADERC
#include <shaderc/shaderc.hpp>
#endif // CR_HAVE_SHADERC

#ifdef CR_HAVE_SHADERC
static std::string crGlCompileSingleShader(const std::string& path, const shaderc_shader_kind kind) {
    std::string sourceCode = Car::readFile(path);
    shaderc::CompileOptions options;
    shaderc::Compiler compiler;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetSourceLanguage(shaderc_source_language_glsl);

#if defined(CR_OPENGL)
    options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
#elif defined(CR_VULKAN)
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
#endif

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(sourceCode, kind, path.c_str(), options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw std::runtime_error(result.GetErrorMessage());
    }

    std::vector<uint32_t> res(result.cbegin(), result.cend());

    return std::string(reinterpret_cast<char*>(res.data()), res.size() * sizeof(uint32_t));
}
#endif // CR_HAVE_SHADERC

namespace Car {
    Ref<Shader> Shader::Create(const std::string& vertexShaderName, const std::string& fragmeantShaderName) {
        std::string vertexBinary;
        std::string fragmeantBinary;

        std::filesystem::path shadersPath =
            (std::filesystem::path)ResourceManager::getResourceDirectory() / ResourceManager::getShadersSubdirectory();
        std::filesystem::path cacheShaderDir = shadersPath / "__CACHE__";

        std::string vertPath = shadersPath / vertexShaderName;
        std::string fragPath = shadersPath / fragmeantShaderName;
        std::filesystem::path vertCacheFile = std::string(cacheShaderDir / vertexShaderName) + ".spv";
        std::filesystem::path fragCacheFile = std::string(cacheShaderDir / fragmeantShaderName) + ".spv";

        if (!std::filesystem::exists(vertCacheFile)) {
#ifdef CR_HAVE_SHADERC
            std::filesystem::create_directories(vertCacheFile.parent_path());
            vertexBinary = crGlCompileSingleShader(vertPath, shaderc_vertex_shader);

            writeToFile(vertCacheFile, reinterpret_cast<const uint8_t*>(vertexBinary.data()), vertexBinary.size());
#else
            CR_CORE_ERROR("Can not online compile shaders without shaderc");
#endif // CR_HAVE_SHADERC
        } else {
            CR_CORE_DEBUG("Loading pre-compiled SPIRV vertex shader");
            std::string data = readFile(vertCacheFile);
            vertexBinary = {data.cbegin(), data.cend()};
        }

        if (!std::filesystem::exists(fragCacheFile)) {
#ifdef CR_HAVE_SHADERC
            std::filesystem::create_directories(fragCacheFile.parent_path());
            fragmeantBinary = crGlCompileSingleShader(fragPath, shaderc_fragment_shader);

            writeToFile(fragCacheFile, reinterpret_cast<const uint8_t*>(fragmeantBinary.data()),
                        fragmeantBinary.size() * sizeof(fragmeantBinary[0]));
#else
            CR_CORE_ERROR("Can not online compile shaders without shaderc");
#endif // CR_HAVE_SHADERC
        } else {
            CR_CORE_DEBUG("Loading pre-compiled SPIRV fragmeant shader");
            std::string data = readFile(fragCacheFile);
            fragmeantBinary = {data.cbegin(), data.cend()};
        }

        return Shader::CreateImpl(vertexBinary, fragmeantBinary);
    }
} // namespace Car
