#pragma once

#include "Car/Renderer/IndexBuffer.hpp"

namespace Car {
    class VulkanIndexBuffer : public IndexBuffer {
    public:
        VulkanIndexBuffer(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type);
        VulkanIndexBuffer(uint32_t count, Buffer::Usage usage, Buffer::Type type);
        virtual ~VulkanIndexBuffer() override;

        virtual void bind() const override;
        virtual void unbind() const override;

        virtual uint32_t getCount() const override { return mCount; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }
        virtual Buffer::Type getType() const override { return mType; }

        virtual void updateData(void* data, uint32_t count, uint32_t offset) override;

    private:
        uint32_t mCount;
        Buffer::Usage mUsage;
        Buffer::Type mType;
    };
} // namespace Car
