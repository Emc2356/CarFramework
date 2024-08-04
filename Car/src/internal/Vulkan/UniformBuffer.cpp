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

        VkDeviceSize bufferSize = size;

        mBuffers.resize(mGraphicsContext->getMaxFramesInFlight());
        mBuffersMemory.resize(mGraphicsContext->getMaxFramesInFlight());
        mBuffersMapped.resize(mGraphicsContext->getMaxFramesInFlight());

        for (size_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
            mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &mBuffers[i], &mBuffersMemory[i]);

            vkMapMemory(mGraphicsContext->getDevice(), mBuffersMemory[i], 0, bufferSize, 0, &mBuffersMapped[i]);
        }
    }

    void VulkanUniformBuffer::createDescriptorLayouts() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = mBinding;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(mGraphicsContext->getDevice(), &layoutInfo, nullptr, &mDescriptorSetLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
        // }

        // void VulkanUniformBuffer::createDescriptorSets() {
        uint32_t maxFramesInFlight = mGraphicsContext->getMaxFramesInFlight();
        std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, mDescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = mGraphicsContext->getDescriptorPool();
        allocInfo.descriptorSetCount = maxFramesInFlight;
        allocInfo.pSetLayouts = layouts.data();

        mDescriptorSets.resize(maxFramesInFlight);
        if (vkAllocateDescriptorSets(mGraphicsContext->getDevice(), &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        VkDevice device = mGraphicsContext->getDevice();

        vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);

        for (size_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
            vkDestroyBuffer(device, mBuffers[i], nullptr);
            vkFreeMemory(device, mBuffersMemory[i], nullptr);
        }
    }

    void VulkanUniformBuffer::setData(const void* data, uint32_t offset /*=0*/) {
        UNUSED(offset);
        memcpy(mBuffersMapped[mGraphicsContext->getImageIndex()], data, mSize);

        for (size_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = mBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = mSize;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = mDescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;

            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;

            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;       // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(mGraphicsContext->getDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }

    Ref<Car::UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding, Buffer::Usage usage) {
        return createRef<VulkanUniformBuffer>(size, binding, usage);
    }
} // namespace Car
