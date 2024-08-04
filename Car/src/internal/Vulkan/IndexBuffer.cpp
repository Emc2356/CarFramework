#include "Car/internal/Vulkan/IndexBuffer.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/Renderer/GraphicsContext.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

namespace Car {
    VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        CR_ASSERT(type == Buffer::Type::UnsignedInt || type == Buffer::Type::UnsignedShort,
                  "Invalid type for IndexBuffer, only UnsignedInt and UnsignedShort is premited");

        UNUSED(data);
        mCount = count;
        mUsage = usage;
        mType = type;

        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());
        mVkIndexType = type == Buffer::Type::UnsignedInt ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;

        bool shouldFree = data == nullptr;

        uint32_t size = Buffer::sizeOfType(mType) * count;

        if (data == nullptr) {
            data = calloc(size, 1);
        }

        VkDevice device = mGraphicsContext->getDevice();
        VkDeviceSize bufferSize = size;

        switch (mUsage) {
        case Buffer::Usage::DynamicDraw: {
            mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &mBuffer, &mBufferMemory);

            vkBindBufferMemory(device, mBuffer, mBufferMemory, 0);

            void* mappedData;
            vkMapMemory(device, mBufferMemory, 0, size, 0, &mappedData);
            memcpy(mappedData, data, (size_t)size);
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
            vkMapMemory(device, stagingBufferMemory, 0, size, 0, &mappedData);
            memcpy(mappedData, data, (size_t)size);
            vkUnmapMemory(device, stagingBufferMemory);

            mGraphicsContext->createBuffer(bufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
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

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        VkDevice device = mGraphicsContext->getDevice();

        vkDeviceWaitIdle(device);
        vkDestroyBuffer(device, mBuffer, nullptr);
        vkFreeMemory(device, mBufferMemory, nullptr);
    }

    void VulkanIndexBuffer::bind() const {
        vkCmdBindIndexBuffer(mGraphicsContext->getCurrentRenderCommandBuffer(), mBuffer, 0, mVkIndexType);
    }

    void VulkanIndexBuffer::unbind() const {}

    void VulkanIndexBuffer::updateData(void* data, uint32_t count, uint32_t offset) {
        UNUSED(data);
        UNUSED(count);
        UNUSED(offset);

        throw std::runtime_error("Car::VulkanIndexBuffer::updateData() not implemented yet");
    }

    Ref<IndexBuffer> IndexBuffer::Create(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        return createRef<VulkanIndexBuffer>(data, count, usage, type);
    }
} // namespace Car
