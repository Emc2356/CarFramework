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

            vkBindBufferMemory(device, mBuffers[i], mBuffersMemory[i], 0);

            vkMapMemory(device, mBuffersMemory[i], 0, mSize, 0, &mBuffersMapped[i]);
        }
    }

    VkDescriptorSetLayoutBinding VulkanUniformBuffer::getDescriptorSetLayout(bool useInVertexShader,
                                                                             bool useInFragmeantShader) {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = mBinding;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;

        CR_IF (!useInVertexShader && !useInFragmeantShader) {
            CR_CORE_ERROR("A uniform buffer most be used in at least one stage");
        }

        if (useInVertexShader) {
            uboLayoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        if (useInFragmeantShader) {
            uboLayoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        uboLayoutBinding.pImmutableSamplers = nullptr;

        return uboLayoutBinding;
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

    void VulkanUniformBuffer::setData(const void* data, uint32_t offset /*=0*/) {
        UNUSED(offset);

        memcpy(mBuffersMapped[mGraphicsContext->getCurrentFrameIndex()], data, mSize);
    }

    Ref<Car::UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding, Buffer::Usage usage) {
        return createRef<VulkanUniformBuffer>(size, binding, usage);
    }
} // namespace Car
