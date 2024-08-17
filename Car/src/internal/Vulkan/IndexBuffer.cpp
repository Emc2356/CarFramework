#include "Car/internal/Vulkan/IndexBuffer.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/Renderer/GraphicsContext.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include <stdexcept>

namespace Car {
    VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint32_t size, Buffer::Usage usage, Buffer::Type type) {
        CR_ASSERT(type == Buffer::Type::UnsignedInt || type == Buffer::Type::UnsignedShort,
                  "Invalid type for IndexBuffer, only UnsignedInt and UnsignedShort is premited");

        CR_IF (true) {
            switch (type) {
            case Buffer::Type::UnsignedInt: {
                if (size % 4 != 0) {
                    throw std::runtime_error("Car::IndexBuffer::Create(data, size, usage, type), size must be "
                                             "devisable by 4 if the type is Car::Buffer::Type::UnsignedInt");
                }
                break;
            }
            case Buffer::Type::UnsignedShort: {
                if (size % 2 != 0) {
                    throw std::runtime_error("Car::IndexBuffer::Create(data, size, usage, type), size must be "
                                             "devisable by 2 if the type is Car::Buffer::Type::UnsignedShort");
                }
                break;
            }
            default: {
                throw std::runtime_error(fmt::format("internal error in line {}", __LINE__));
                break;
            }
            }
        }

        UNUSED(data);
        mUsage = usage;
        mType = type;
        mSize = size;
        mCount = size / Buffer::sizeOfType(mType);

        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());
        mVkIndexType = type == Buffer::Type::UnsignedInt ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;

        bool shouldFree = data == nullptr;

        if (data == nullptr) {
            data = calloc(mSize, 1);
        }

        VkDevice device = mGraphicsContext->getDevice();
        VkDeviceSize bufferSize = mSize;

        switch (mUsage) {
        case Buffer::Usage::DynamicDraw: {
            mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           &mBuffer, &mBufferMemory);

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

            void* mappedData;
            vkMapMemory(device, stagingBufferMemory, 0, mSize, 0, &mappedData);
            memcpy(mappedData, data, (size_t)mSize);
            vkUnmapMemory(device, stagingBufferMemory);

            mGraphicsContext->createBuffer(bufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
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

    void VulkanIndexBuffer::releaseDeviceObjects() {
        VkDevice device = mGraphicsContext->getDevice();

        vkDeviceWaitIdle(device);
        mGraphicsContext->freeBuffer(&mBuffer, &mBufferMemory);
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() { releaseDeviceObjects(); }

    void VulkanIndexBuffer::bind() const {
        vkCmdBindIndexBuffer(mGraphicsContext->getCurrentRenderCommandBuffer(), mBuffer, 0, mVkIndexType);
    }

    void VulkanIndexBuffer::updateData(void* data, uint32_t size, uint32_t offset) {
        CR_IF (data == nullptr) {
            CR_CORE_ERROR("Car::VertexBuffer::updateData(data, size, offset), data can not be a null pointer");
        }
        CR_IF (size == 0) {
            CR_CORE_ERROR("Car::VertexBuffer::updateData(data, size, offset), size can not be 0");
        }

        CR_IF (true) {
            switch (mType) {
            case Buffer::Type::UnsignedInt: {
                if (size % 4 != 0) {
                    throw std::runtime_error("Car::IndexBuffer::updateData(data, size, offset), size must be devisable "
                                             "by 4 if the type is Car::Buffer::Type::UnsignedInt");
                }
                break;
            }
            case Buffer::Type::UnsignedShort: {
                if (size % 2 != 0) {
                    throw std::runtime_error("Car::IndexBuffer::updateData(data, size, offset), size must be devisable "
                                             "by 2 if the type is Car::Buffer::Type::UnsignedShort");
                }
                break;
            }
            default: {
                throw std::runtime_error(fmt::format("internal error in line {}", __LINE__));
                break;
            }
            }
        }

        CR_IF (true) {
            switch (mType) {
            case Buffer::Type::UnsignedInt: {
                if (offset % 4 != 0) {
                    throw std::runtime_error("Car::IndexBuffer::updateData(data, size, offset), offset must be "
                                             "devisable by 4 if the type is Car::Buffer::Type::UnsignedInt");
                }
                break;
            }
            case Buffer::Type::UnsignedShort: {
                if (offset % 2 != 0) {
                    throw std::runtime_error("Car::IndexBuffer::updateData(data, size, offset), offset must be "
                                             "devisable by 2 if the type is Car::Buffer::Type::UnsignedShort");
                }
                break;
            }
            default: {
                throw std::runtime_error(fmt::format("internal error in line {}", __LINE__));
                break;
            }
            }
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
                    throw std::runtime_error(
                        fmt::format("internal error in Car::VulkanIndexBuffer::updateData() in line {}.", __LINE__));
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
                    mGraphicsContext->createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
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
                                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mBuffer, &mBufferMemory);

                    mGraphicsContext->copyBuffer(stagingBuffer, mBuffer, bufferSize, 0, 0);

                    vkDestroyBuffer(device, stagingBuffer, nullptr);
                    vkFreeMemory(device, stagingBufferMemory, nullptr);
                    break;
                }
                default: {
                    throw std::runtime_error(
                        fmt::format("internal error in Car::VulkanIndexBuffer::updateData() in line {}.", __LINE__));
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
                    throw std::runtime_error(
                        fmt::format("internal error in Car::VulkanIndexBuffer::updateData() in line {}.", __LINE__));
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

                    mGraphicsContext->createBuffer(mSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
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
                    throw std::runtime_error(
                        fmt::format("internal error in Car::VulkanIndexBuffer::updateData() in line {}.", __LINE__));
                    break;
                }
                }
            }
        }

        mCount = mSize / Buffer::sizeOfType(mType);
    }

    Ref<IndexBuffer> IndexBuffer::Create(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        return createRef<VulkanIndexBuffer>(data, count, usage, type);
    }
} // namespace Car
