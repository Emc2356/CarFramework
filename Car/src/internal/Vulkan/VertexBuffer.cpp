#include "Car/internal/Vulkan/VertexBuffer.hpp"
#include "Car/Core/Core.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <cstring>
#include <glad/vulkan.h>
#include <stdexcept>

namespace Car {

    VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size, Buffer::Usage usage) {
        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());
        mUsage = usage;
        mSize = size;
        mUsage = usage;

        bool shouldFree = data == nullptr;

        if (data == nullptr) {
            data = calloc(size, 1);
        }

        VkDevice device = mGraphicsContext->getDevice();
        VkDeviceSize bufferSize = mSize;

        switch (mUsage) {
        case Buffer::Usage::DynamicDraw: {
            mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &mBuffer, &mBufferMemory);

            void* mappedData;
            vkMapMemory(device, mBufferMemory, 0, mSize, 0, &mappedData);
            std::memcpy(mappedData, data, (size_t)mSize);
            vkUnmapMemory(device, mBufferMemory);
            break;
        }
        case Buffer::Usage::StaticDraw: {
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &stagingBuffer, &stagingBufferMemory);

            void* mappedData;
            vkMapMemory(device, stagingBufferMemory, 0, mSize, 0, &mappedData);
            std::memcpy(mappedData, data, (size_t)mSize);
            vkUnmapMemory(device, stagingBufferMemory);

            mGraphicsContext->createBuffer(bufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mBuffer, &mBufferMemory);

            mGraphicsContext->copyBuffer(stagingBuffer, mBuffer, bufferSize, 0, 0);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
            break;
        }
        default: {
            throw std::runtime_error(
                "Unrecognized usage passed to Car::VertexBuffer::Create(data, size, format, usage)");
            break;
        }
        }

        if (shouldFree) {
            free(data);
        }
    }

    void VulkanVertexBuffer::releaseDeviceObjects() {
        VkDevice device = mGraphicsContext->getDevice();
        vkDeviceWaitIdle(device);
        vkDestroyBuffer(device, mBuffer, nullptr);
        vkFreeMemory(device, mBufferMemory, nullptr);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() { releaseDeviceObjects(); }

    void VulkanVertexBuffer::bind() const {
        VkBuffer vertexBuffers[] = {mBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(mGraphicsContext->getCurrentRenderCommandBuffer(), 0, 1, vertexBuffers, offsets);
    }

    void VulkanVertexBuffer::updateData(void* data, uint32_t size, uint32_t offset) {
        CR_IF (data == nullptr) {
            CR_CORE_ERROR("Car::VertexBuffer::updateData(data, size, offset), data can not be a null pointer");
        }
        CR_IF (size == 0) {
            CR_CORE_ERROR("Car::VertexBuffer::updateData(data, size, offset), size can not be 0");
        }

        VkDevice device = mGraphicsContext->getDevice();

        if (offset == 0) {
            if (size <= mSize) {
                switch (mUsage) {
                case Buffer::Usage::DynamicDraw: {
                    void* mappedData;
                    vkMapMemory(device, mBufferMemory, 0, size, 0, &mappedData);
                    std::memcpy(mappedData, data, (size_t)size);
                    vkUnmapMemory(device, mBufferMemory);
                    break;
                }
                case Buffer::Usage::StaticDraw: {
                    VkBuffer stagingBuffer;
                    VkDeviceMemory stagingBufferMemory;
                    mGraphicsContext->createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                   &stagingBuffer, &stagingBufferMemory);

                    void* mappedData;
                    vkMapMemory(device, stagingBufferMemory, 0, size, 0, &mappedData);
                    std::memcpy(mappedData, data, (size_t)size);
                    vkUnmapMemory(device, stagingBufferMemory);

                    mGraphicsContext->copyBuffer(stagingBuffer, mBuffer, size, 0, 0);

                    vkDestroyBuffer(device, stagingBuffer, nullptr);
                    vkFreeMemory(device, stagingBufferMemory, nullptr);
                    break;
                }
                default: {
                    throw std::runtime_error("internal error in Car::VulkanVertexBuffer::updateData() in line" + std::to_string(__LINE__));
                    break;
                }
                }
            } else {
                vkDestroyBuffer(device, mBuffer, nullptr);
                vkFreeMemory(device, mBufferMemory, nullptr);
                mSize = size;
                VkDeviceSize bufferSize = size;

                switch (mUsage) {
                case Buffer::Usage::DynamicDraw: {
                    mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                   &mBuffer, &mBufferMemory);

                    void* mappedData;
                    vkMapMemory(device, mBufferMemory, 0, mSize, 0, &mappedData);
                    std::memcpy(mappedData, data, (size_t)mSize);
                    vkUnmapMemory(device, mBufferMemory);
                    break;
                }
                case Buffer::Usage::StaticDraw: {
                    VkBuffer stagingBuffer;
                    VkDeviceMemory stagingBufferMemory;
                    mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                   &stagingBuffer, &stagingBufferMemory);

                    void* mappedData;
                    vkMapMemory(device, stagingBufferMemory, 0, mSize, 0, &mappedData);
                    std::memcpy(mappedData, data, (size_t)mSize);
                    vkUnmapMemory(device, stagingBufferMemory);

                    mGraphicsContext->createBuffer(bufferSize,
                                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mBuffer, &mBufferMemory);

                    mGraphicsContext->copyBuffer(stagingBuffer, mBuffer, bufferSize, 0, 0);

                    vkDestroyBuffer(device, stagingBuffer, nullptr);
                    vkFreeMemory(device, stagingBufferMemory, nullptr);
                    break;
                }
                default: {
                    throw std::runtime_error("internal error in Car::VulkanVertexBuffer::updateData() in line" + std::to_string(__LINE__));
                    break;
                }
                }
            }
        } else {
            if (offset + size <= mSize) {
                switch (mUsage) {
                case Buffer::Usage::DynamicDraw: {
                    void* mappedData;
                    vkMapMemory(device, mBufferMemory, offset, size, 0, &mappedData);
                    std::memcpy(mappedData, data, (size_t)size);
                    vkUnmapMemory(device, mBufferMemory);
                    break;
                }
                case Buffer::Usage::StaticDraw: {
                    VkBuffer stagingBuffer;
                    VkDeviceMemory stagingBufferMemory;
                    mGraphicsContext->createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                   &stagingBuffer, &stagingBufferMemory);

                    void* mappedData;
                    vkMapMemory(device, stagingBufferMemory, offset, size, 0, &mappedData);
                    std::memcpy(mappedData, data, (size_t)size);
                    vkUnmapMemory(device, stagingBufferMemory);

                    mGraphicsContext->copyBuffer(stagingBuffer, mBuffer, size, 0, offset);

                    vkDestroyBuffer(device, stagingBuffer, nullptr);
                    vkFreeMemory(device, stagingBufferMemory, nullptr);
                    break;
                }
                default: {
                    throw std::runtime_error("internal error in Car::VulkanVertexBuffer::updateData() in line" + std::to_string(__LINE__));
                    break;
                }
                }
            } else {
                switch (mUsage) {
                case Buffer::Usage::DynamicDraw: {
                    uint8_t* temp;
                    if (offset <= mSize) {
                        temp = (uint8_t*)std::malloc(offset + size);

                        void* mappedData;
                        vkMapMemory(device, mBufferMemory, 0, offset, 0, &mappedData);
                        std::memcpy(temp, mappedData, (size_t)offset);
                        vkUnmapMemory(device, mBufferMemory);

                        std::memcpy(temp + offset, data, size);

                        mSize = offset + size;
                    } else {
                        temp = (uint8_t*)std::malloc(mSize + (mSize - offset) + size);

                        std::memset(temp + mSize - 1, 0, mSize - offset + 2);

                        void* mappedData;
                        vkMapMemory(device, mBufferMemory, 0, mSize, 0, &mappedData);
                        std::memcpy(temp, mappedData, (size_t)mSize);
                        vkUnmapMemory(device, mBufferMemory);

                        std::memcpy(temp + offset, data, size);

                        mSize = mSize + (mSize - offset) + size;
                    }

                    releaseDeviceObjects();

                    mGraphicsContext->createBuffer(mSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                   &mBuffer, &mBufferMemory);

                    void* mappedData;
                    vkMapMemory(device, mBufferMemory, 0, mSize, 0, &mappedData);
                    std::memcpy(mappedData, temp, (size_t)mSize);
                    vkUnmapMemory(device, mBufferMemory);

                    std::free(temp);
                    break;
                }
                case Buffer::Usage::StaticDraw: {
                    throw std::runtime_error(
                        "Car::VertexBuffer::updateData(data, size, offset), if the VertexBuffer was created with the "
                        "static usage the internal buffer can not be resized if (offset > 0 && ffset + size > mSize). "
                        "Consider allocating more memory ahead of time");
                    break;
                }
                default: {
                    throw std::runtime_error("internal error in Car::VulkanVertexBuffer::updateData() in line" + std::to_string(__LINE__));
                    break;
                }
                }
            }
        }
    }

    Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, Buffer::Usage usage) {
        return createRef<VulkanVertexBuffer>(data, size, usage);
    }
} // namespace Car
