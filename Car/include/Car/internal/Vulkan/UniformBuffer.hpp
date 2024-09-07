#pragma once

#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <glad/vulkan.h>

namespace Car {
    class VulkanUniformBuffer : public UniformBuffer {
    public:
        VulkanUniformBuffer(uint64_t size, Buffer::Usage usage);
        ~VulkanUniformBuffer();

        virtual uint64_t getSize() const override { return mSize; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }

        virtual void setData(const void* data) override;

        // for which frame in flight to retrieve the VlDescriptorBufferInfo
        VkDescriptorBufferInfo getDescriptorBufferInfo(uint32_t i);

    private:
        uint64_t mSize;
        Buffer::Usage mUsage;

        std::vector<VkBuffer> mBuffers;
        std::vector<VkDeviceMemory> mBuffersMemory;
        std::vector<void*> mBuffersMapped;

        Ref<VulkanGraphicsContext> mGraphicsContext;
    };
} // namespace Car
