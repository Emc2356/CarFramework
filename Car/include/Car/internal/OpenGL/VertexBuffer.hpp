#pragma once

#include "Car/Renderer/VertexBuffer.hpp"


namespace Car {
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type);
        OpenGLVertexBuffer(uint32_t count, Buffer::Usage usage, Buffer::Type type);
        virtual ~OpenGLVertexBuffer() override;

        virtual void setLayout(const BufferLayout& layout) override;

        virtual void bind() const override;
        virtual void unbind() const override;

        virtual void updateData(void* data, uint32_t count, uint32_t offset) override;

        void applyLayout() const;
    private:
        uint32_t mID;
        uint32_t mOpenGLUsage;
    };
}
