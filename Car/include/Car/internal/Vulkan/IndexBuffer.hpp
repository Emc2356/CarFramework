#pragma once

#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

namespace Car {
    class VulkanIndexBuffer : public IndexBuffer {
    public:
        VulkanIndexBuffer(void* data, uint32_t size, Buffer::Usage usage, Buffer::Type type);
        virtual ~VulkanIndexBuffer() override;

        void releaseDeviceObjects();

        virtual void bind() const override;

        virtual uint32_t getCount() const override { return mCount; }
        virtual uint32_t getSize() const override { return mSize; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }
        virtual Buffer::Type getType() const override { return mType; }

        virtual void updateData(void* data, uint32_t size, uint32_t offset) override;

    private:
        uint32_t mCount;
        uint32_t mSize;
        Buffer::Usage mUsage;
        Buffer::Type mType;

        Ref<VulkanGraphicsContext> mGraphicsContext;
        VkBuffer mBuffer;
        VkDeviceMemory mBufferMemory;
        VkIndexType mVkIndexType;
    };
} // namespace Car
