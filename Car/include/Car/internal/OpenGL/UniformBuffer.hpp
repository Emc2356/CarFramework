#pragma once

#include "Car/Renderer/UniformBuffer.hpp"


namespace Car {
    class OpenGLUniformBuffer : public UniformBuffer {
    public:
        OpenGLUniformBuffer(uint32_t size, uint32_t binding, Buffer::Usage usage);
        ~OpenGLUniformBuffer();

        virtual void setData(const void* data, uint32_t offset=0) override;
    private:
        uint32_t mID;
        uint32_t mOpenGLUsage;
    };
}
