#pragma once

#include "Car/Renderer/IndexBuffer.hpp"

namespace Car {
    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type);
        OpenGLIndexBuffer(uint32_t count, Buffer::Usage usage, Buffer::Type type);
        virtual ~OpenGLIndexBuffer() override;

        virtual void bind() const override;
        virtual void unbind() const override;

        virtual uint32_t getCount() const override { return mCount; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }
        virtual Buffer::Type getType() const override { return mType; }

        uint32_t getOpenGLType() const { return mOpenGLType; }

        virtual void updateData(void* data, uint32_t count, uint32_t offset) override;

    private:
        uint32_t mCount;
        Buffer::Usage mUsage;
        Buffer::Type mType;

        uint32_t mID;
        uint32_t mOpenGLUsage;
        uint32_t mOpenGLType;
    };
} // namespace Car
