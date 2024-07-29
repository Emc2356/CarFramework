#pragma once

#include "Car/Renderer/VertexBuffer.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include <glad/vulkan.h>


namespace Car {
    class VulkanVertexBuffer : public VertexBuffer {
    public:
        VulkanVertexBuffer(void* data, uint32_t size, BufferLayout layout, Buffer::Usage usage);
        VulkanVertexBuffer(uint32_t size, Buffer::Usage usage);
        virtual ~VulkanVertexBuffer() override;

        virtual const BufferLayout& getLayout() const override { return mLayout; }

        virtual uint32_t getSize() const override { return mSize; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }

        virtual void bind() const override;
        virtual void unbind() const override;

        virtual void updateData(void* data, uint32_t size, uint32_t offset) override;

        const VkVertexInputBindingDescription& getBindingDescription() const { return mBindingDescription; }
        const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const { return mAttributeDescriptions; }
    private:
        BufferLayout mLayout;
        uint32_t mSize;
        Buffer::Usage mUsage;
        
        Ref<VulkanGraphicsContext> mGraphicsContext;
        VkVertexInputBindingDescription mBindingDescription;
        std::vector<VkVertexInputAttributeDescription> mAttributeDescriptions;
        VkBuffer mBuffer;
        VkDeviceMemory mBufferMemory;
    };
} // namespace Car
