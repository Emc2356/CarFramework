#pragma once

#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <glad/vulkan.h>

namespace Car {
    class VulkanUniformBuffer : public UniformBuffer {
    public:
        VulkanUniformBuffer(uint32_t size, uint32_t binding, Buffer::Usage usage);
        ~VulkanUniformBuffer();

        virtual uint32_t getSize() const override { return mSize; }
        virtual uint32_t getBinding() const override { return mBinding; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }

        virtual void setData(const void* data, uint32_t offset = 0) override;

        VkDescriptorSetLayoutBinding getDescriptorSetLayout(bool useInVertexShader, bool useInFragmeantShader);
        VkDescriptorBufferInfo getDescriptorBufferInfo(uint32_t i);
    private:
        uint32_t mSize;
        uint32_t mBinding;
        Buffer::Usage mUsage;

        std::vector<VkBuffer> mBuffers;
        std::vector<VkDeviceMemory> mBuffersMemory;
        std::vector<void*> mBuffersMapped;

        Ref<VulkanGraphicsContext> mGraphicsContext;
    };
} // namespace Car
