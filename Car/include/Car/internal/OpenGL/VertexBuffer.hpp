#pragma once

#include "Car/Renderer/VertexBuffer.hpp"

namespace Car {
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(void* data, uint32_t size, BufferLayout layout, Buffer::Usage usage);
        virtual ~OpenGLVertexBuffer() override;

        virtual const BufferLayout& getLayout() const override { return mLayout; }

        virtual uint32_t getSize() const override { return mSize; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }

        virtual void bind() const override;
        virtual void unbind() const override;

        virtual void updateData(void* data, uint32_t size, uint32_t offset) override;

        void applyLayout() const;

    private:
        BufferLayout mLayout;
        uint32_t mSize;
        Buffer::Usage mUsage;
        // OpenGL specific
        uint32_t mID;
        uint32_t mOpenGLUsage;
    };
} // namespace Car
