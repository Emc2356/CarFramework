#pragma once

#include "Car/Renderer/VertexBuffer.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include <glad/vulkan.h>

namespace Car {
    class VulkanVertexBuffer : public VertexBuffer {
    public:
        VulkanVertexBuffer(void* data, uint32_t size, Buffer::Usage usage);
        VulkanVertexBuffer(uint32_t size, Buffer::Usage usage);
        virtual ~VulkanVertexBuffer() override;

        void releaseDeviceObjects();

        virtual uint32_t getSize() const override { return mSize; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }

        virtual void bind() const override;
        virtual void unbind() const override {}

        virtual void updateData(void* data, uint32_t size, uint32_t offset) override;

    private:
        uint32_t mSize;
        Buffer::Usage mUsage;

        Ref<VulkanGraphicsContext> mGraphicsContext;
        VkBuffer mBuffer;
        VkDeviceMemory mBufferMemory;
    };
} // namespace Car
