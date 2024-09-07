#pragma once

#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

namespace Car {
    class VulkanIndexBuffer : public IndexBuffer {
    public:
        VulkanIndexBuffer(void* data, uint64_t size, Buffer::Usage usage, Buffer::Type type);
        virtual ~VulkanIndexBuffer() override;

        void releaseDeviceObjects();

        virtual void bind() const override;

        virtual uint64_t getCount() const override { return mCount; }
        virtual uint64_t getSize() const override { return mSize; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }
        virtual Buffer::Type getType() const override { return mType; }

        virtual void updateData(void* data, uint64_t size, uint64_t offset) override;

    private:
        uint64_t mCount;
        uint64_t mSize;
        Buffer::Usage mUsage;
        Buffer::Type mType;

        Ref<VulkanGraphicsContext> mGraphicsContext;
        VkBuffer mBuffer;
        VkDeviceMemory mBufferMemory;
        VkIndexType mVkIndexType;
    };
} // namespace Car
