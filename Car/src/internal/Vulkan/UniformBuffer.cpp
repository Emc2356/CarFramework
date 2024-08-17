#include "Car/internal/Vulkan/UniformBuffer.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <glad/vulkan.h>

namespace Car {
    VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding, Buffer::Usage usage) {
        mSize = size;
        mBinding = binding;
        mUsage = usage;

        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());

        mBuffers.resize(mGraphicsContext->getMaxFramesInFlight());
        mBuffersMemory.resize(mGraphicsContext->getMaxFramesInFlight());
        mBuffersMapped.resize(mGraphicsContext->getMaxFramesInFlight());

        VkDevice device = mGraphicsContext->getDevice();

        for (size_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
            mGraphicsContext->createBuffer(mSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &mBuffers[i], &mBuffersMemory[i]);

            vkMapMemory(device, mBuffersMemory[i], 0, mSize, 0, &mBuffersMapped[i]);
        }
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        VkDevice device = mGraphicsContext->getDevice();
        for (size_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
            vkDestroyBuffer(device, mBuffers[i], nullptr);
            vkFreeMemory(device, mBuffersMemory[i], nullptr);
        }
    }

    VkDescriptorBufferInfo VulkanUniformBuffer::getDescriptorBufferInfo(uint32_t i) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = mBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = mSize;

        return bufferInfo;
    }

    void VulkanUniformBuffer::setData(const void* data) {
        std::memcpy(mBuffersMapped[mGraphicsContext->getCurrentFrameIndex()], data, mSize);
    }

    Ref<Car::UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding, Buffer::Usage usage) {
        return createRef<VulkanUniformBuffer>(size, binding, usage);
    }
} // namespace Car
