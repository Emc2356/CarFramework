#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <Car/internal/Vulkan/CompiledShader.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + path);
    }
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    std::string buffer(size, ' ');
    file.seekg(0);
    file.read(&buffer[0], size);
    return buffer;
}

void writeToFile(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream wf(path, std::ios::out | std::ios::trunc);

    if (!wf) {
        throw std::runtime_error("failed to open file: " + path);
    }

    wf.write(reinterpret_cast<const char*>(data.data()), data.size());

    wf.close();

    if (!wf.good()) {
        throw std::runtime_error("error occured at writing time! " + path);
    }
}

std::string compileSingleShader(const std::string& path, const shaderc_shader_kind kind) {
    std::string sourceCode = readFile(path);
    shaderc::CompileOptions options;
    shaderc::Compiler compiler;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetSourceLanguage(shaderc_source_language_glsl);

    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(sourceCode, kind, path.c_str(), options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw std::runtime_error(result.GetErrorMessage());
    }

    std::vector<uint32_t> res(result.cbegin(), result.cend());

    return std::string(reinterpret_cast<char*>(res.data()), res.size() * sizeof(uint32_t));
}

void fillSetField(Car::SingleCompiledShader* pSCS) {
    pSCS->sets.resize(0);
    spirv_cross::Compiler compiler((uint32_t*)pSCS->shader.data(), pSCS->shader.size() / 4);
    spirv_cross::ShaderResources resources(compiler.get_shader_resources());

    uint32_t maxDescriptorSetCount = 0;
    for (const auto& uniformBuffer : resources.uniform_buffers) {
        maxDescriptorSetCount =
            MAX(compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet) + 1, maxDescriptorSetCount);
    }
    for (const auto& uniformBuffer : resources.sampled_images) {
        maxDescriptorSetCount =
            MAX(compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet) + 1, maxDescriptorSetCount);
    }

    if (maxDescriptorSetCount > 4) {
        throw std::runtime_error("A shader can have max of 4 sets per the vulkan spec");
    }

    pSCS->sets.resize(maxDescriptorSetCount);

    for (const auto& resource : resources.uniform_buffers) {
        uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        uint8_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        pSCS->sets[set].push_back({
            binding,
            Car::DescriptorType::UniformBuffer,
            Car::DescriptorStage::VertexShader,
        });
    }

    for (const auto& resource : resources.sampled_images) {
        uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        uint8_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        pSCS->sets[set].push_back({
            binding,
            Car::DescriptorType::Sampler2D,
            Car::DescriptorStage::VertexShader,
        });
    }
}


int main(int argc, char** argv) {
    argc--;
    argv++;
    
    if (argc % 2 || argc == 0) {
        std::cout << "shaderCompiler is a simple program similar to glslc but it dumps out data optimal for car" << std::endl;
        std::cout << "it is meant to be run from the shaders folder in the resource folder" << std::endl;
        std::cerr << "Usage: <filename1 (vert|frag)> [filename2 (vert|frag) ...]" << std::endl;
        return 1;
    }
    
    std::vector<std::string> files;
    std::vector<shaderc_shader_kind> shaderKinds;
    
    while (argc) {        
        std::string file = argv[0];
        if (!std::filesystem::exists(file)) {
            std::cerr << "file " << file << " doesnt exist" << std::endl;
        }
        
        files.push_back(file);
        
        std::string kindStr = argv[1];
        
        if (kindStr == "vert") {
            shaderKinds.push_back(shaderc_vertex_shader);
        } else if (kindStr == "frag") {
            shaderKinds.push_back(shaderc_fragment_shader);
        } else {
            std::cerr << "shader kind " << kindStr << " is not vert nor frag" << std::endl;
        }
        
        argc -= 2;
        argv += 2;
    }
    
    std::filesystem::path cacheDir = "__CACHE__";
    if (!std::filesystem::exists(cacheDir)) {
        std::filesystem::create_directories(cacheDir);
    }
    
    for (uint32_t i = 0; i < files.size(); i++) {
        std::string targetFile = files[i];
        shaderc_shader_kind shaderKind = shaderKinds[i];
        std::cout << "compiling " << targetFile << std::endl;
        
        std::string contents = readFile(targetFile);
        
        Car::SingleCompiledShader compiledShader;
        compiledShader.shader = compileSingleShader(targetFile, shaderKind);
        fillSetField(&compiledShader);
        
        std::filesystem::path outFile = std::string(cacheDir / targetFile) + ".crss";
        
        std::filesystem::create_directories(outFile.parent_path());
        
        writeToFile(std::string(outFile), compiledShader.toBytes());
    }
    
    
    return 0;
}
