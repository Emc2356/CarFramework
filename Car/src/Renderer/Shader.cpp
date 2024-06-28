#include "Car/Renderer/Shader.hpp"

#include "Car/Utils.hpp"


namespace Car {
    Ref<Shader> Shader::Create(const std::string& vertexShaderName, const std::string& fragmeantShaderName) {
        // TODO: make shaderc work properly, or at least fucking compile
        // so we have something proper and not this shit

        return Shader::CreateImpl(
            readFile("./resources/shaders/" + vertexShaderName),
            readFile("./resources/shaders/" + fragmeantShaderName)
        );;
    }
}
