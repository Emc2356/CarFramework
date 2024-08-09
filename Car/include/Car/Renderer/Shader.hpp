#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/UniformBuffer.hpp"

namespace Car {
    class Shader {
    public:
        virtual ~Shader() = default;

        virtual void bind() const = 0;

        // useful for debug purposes
        virtual void unbind() const = 0;

        virtual void setInput(Ref<UniformBuffer> ub, bool useInVertexShader, bool useInFragmeantShader) = 0;
        
        static Ref<Shader> Create(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);

        // implementation detail
        static Ref<Shader> CreateImpl(const std::string& vertexBinary, const std::string& fragmeantBinary);
    };
} // namespace Car
