#include "Car/internal/Vulkan/VertexBuffer.hpp"
#include "Car/Core/Core.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <cstdlib>
#include <glad/vulkan.h>
#include <stdexcept>

namespace Car {
    CR_FORCE_INLINE VkFormat BufferLayoutDataTypeToVulkanType(BufferLayout::DataType type) {
        switch (type) {
        case BufferLayout::DataType::Float:
            return VK_FORMAT_R32_SFLOAT;
        case BufferLayout::DataType::Float2:
            return VK_FORMAT_R32G32_SFLOAT;
        case BufferLayout::DataType::Float3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case BufferLayout::DataType::Float4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case BufferLayout::DataType::Mat3:
            CR_CORE_ERROR("BufferLayout::DataType::Mat3 not supported as vulkan vertex attribute");
            CR_DEBUGBREAK();
            return VK_FORMAT_UNDEFINED;
        case BufferLayout::DataType::Mat4:
            CR_CORE_ERROR("BufferLayout::DataType::Mat4 not supported as vulkan vertex attribute");
            CR_DEBUGBREAK();
            return VK_FORMAT_UNDEFINED;
        case BufferLayout::DataType::Int:
            return VK_FORMAT_R32_SINT;
        case BufferLayout::DataType::Int2:
            return VK_FORMAT_R32G32_SINT;
        case BufferLayout::DataType::Int3:
            return VK_FORMAT_R32G32B32_SINT;
        case BufferLayout::DataType::Int4:
            return VK_FORMAT_R32G32B32A32_SINT;
        case BufferLayout::DataType::UInt:
            return VK_FORMAT_R32_UINT;
        case BufferLayout::DataType::UInt2:
            return VK_FORMAT_R32G32_UINT;
        case BufferLayout::DataType::UInt3:
            return VK_FORMAT_R32G32B32_UINT;
        case BufferLayout::DataType::UInt4:
            return VK_FORMAT_R32G32B32A32_UINT;
        default:
            CR_CORE_ERROR("Unrecognized BufferLayout::DataType: {0}", static_cast<uint32_t>(type));
            CR_DEBUGBREAK();
            return VK_FORMAT_UNDEFINED;
        }
    }

    VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size, BufferLayout layout, Buffer::Usage usage) {
        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());
        mLayout = layout;
        mUsage = usage;
        mSize = size;
        mUsage = usage;

        bool shouldFree = data == nullptr;

        if (data == nullptr) {
            data = calloc(size, 1);
        }

        mBindingDescription.binding = 0;
        mBindingDescription.stride = layout.getTotalSize();
        mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        const std::vector<BufferLayout::Element>& elements = layout.getElements();
        mAttributeDescriptions.resize(elements.size());

        for (uint32_t i = 0; i < mAttributeDescriptions.size(); i++) {
            mAttributeDescriptions[i].binding = 0;
            // TODO: location might be wrong if ex an element is dvec4
            mAttributeDescriptions[i].location = i;
            mAttributeDescriptions[i].format = BufferLayoutDataTypeToVulkanType(elements[i].type);
            mAttributeDescriptions[i].offset = elements[i].offset;
        }

        VkDevice device = mGraphicsContext->getDevice();
        VkDeviceSize bufferSize = mSize;

        switch (mUsage) {
        case Buffer::Usage::DynamicDraw: {
            mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &mBuffer, &mBufferMemory);

            vkBindBufferMemory(device, mBuffer, mBufferMemory, 0);

            void* mappedData;
            vkMapMemory(device, mBufferMemory, 0, mSize, 0, &mappedData);
            memcpy(mappedData, data, (size_t)mSize);
            vkUnmapMemory(device, mBufferMemory);
            break;
        }
        case Buffer::Usage::StaticDraw: {
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &stagingBuffer, &stagingBufferMemory);

            vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

            void* mappedData;
            vkMapMemory(device, stagingBufferMemory, 0, mSize, 0, &mappedData);
            memcpy(mappedData, data, (size_t)mSize);
            vkUnmapMemory(device, stagingBufferMemory);

            mGraphicsContext->createBuffer(bufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mBuffer, &mBufferMemory);

            vkBindBufferMemory(device, mBuffer, mBufferMemory, 0);

            mGraphicsContext->copyBuffer(stagingBuffer, mBuffer, bufferSize, 0, 0);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
            break;
        }
        default: {
            throw std::runtime_error(
                "Unrecognized usage passedto Car::VertexBuffer::Create(data, size, format, usage)");
            break;
        }
        }

        if (shouldFree) {
            free(data);
        }
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        vkDeviceWaitIdle(mGraphicsContext->getDevice());
        vkDestroyBuffer(mGraphicsContext->getDevice(), mBuffer, nullptr);
        vkFreeMemory(mGraphicsContext->getDevice(), mBufferMemory, nullptr);
    }

    void VulkanVertexBuffer::bind() const {
        VkBuffer vertexBuffers[] = {mBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(mGraphicsContext->getCurrentRenderCommandBuffer(), 0, 1, vertexBuffers, offsets);
    }

    void VulkanVertexBuffer::updateData(void* data, uint32_t size, uint32_t offset) {
        UNUSED(data);
        UNUSED(size);
        UNUSED(offset);

        throw std::runtime_error("Car::VulkanVertexBuffer::updateData() not implemented yet");
    }

    Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, BufferLayout layout, Buffer::Usage usage) {
        return createRef<VulkanVertexBuffer>(data, size, layout, usage);
    }
} // namespace Car
