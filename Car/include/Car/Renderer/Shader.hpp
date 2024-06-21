#pragma once

#include "Car/Core/Core.hpp"


namespace Car {
    class Shader {
    public:
        virtual ~Shader() = default;

        virtual void bind() const = 0;

        // useful for debug purposes
        virtual void unbind() const = 0;

        static Ref<Shader> Create(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);

        // implementation detail
        static Ref<Shader> CreateImpl(const std::string& vertexBinary, const std::string& fragmeantBinary);
    };
}
