#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/Renderer/UniformBuffer.hpp"

namespace Car {
    class Shader {
    public:
        virtual ~Shader() = default;

        virtual void bind() const = 0;

        virtual const ShaderLayoutInput& getInputLayout() const = 0;

        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<UniformBuffer> ub) = 0;
        
        static Ref<Shader> Create(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath, const ShaderLayoutInput& inputLayout);
    };
} // namespace Car
