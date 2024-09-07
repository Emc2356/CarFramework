#pragma once

#include "Car/Renderer/VertexBuffer.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include <glad/vulkan.h>

namespace Car {
    class VulkanVertexBuffer : public VertexBuffer {
    public:
        VulkanVertexBuffer(void* data, uint64_t size, Buffer::Usage usage);
        virtual ~VulkanVertexBuffer() override;

        void releaseDeviceObjects();

        virtual uint64_t getSize() const override { return mSize; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }

        virtual void bind() const override;

        virtual void updateData(void* data, uint64_t size, uint64_t offset) override;

    private:
        uint64_t mSize;
        Buffer::Usage mUsage;

        Ref<VulkanGraphicsContext> mGraphicsContext;
        VkBuffer mBuffer;
        VkDeviceMemory mBufferMemory;
    };
} // namespace Car
