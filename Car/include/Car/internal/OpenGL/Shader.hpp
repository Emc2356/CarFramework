#pragma once

#include "Car/Renderer/Shader.hpp"

namespace Car {
    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& vertexBinary, const std::string& fragmeantBinary);
        OpenGLShader() : mID(0) {}
        virtual ~OpenGLShader() override;

        virtual void bind() const override;
        virtual void unbind() const override;

    private:
        uint32_t mID;
    };
} // namespace Car
