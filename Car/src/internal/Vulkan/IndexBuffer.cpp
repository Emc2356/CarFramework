#include "Car/internal/Vulkan/IndexBuffer.hpp"

namespace Car {
    VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        CR_ASSERT(type == Buffer::Type::UnsignedInt || type == Buffer::Type::UnsignedShort ||
                      type == Buffer::Type::Byte,
                  "Invalid type for IndexBuffer, only UnsignedInt, UnsignedShort "
                  "and Byte is premited");

        UNUSED(data);
        mCount = count;
        mUsage = usage;
        mType = type;
    }

    VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        CR_ASSERT(type == Buffer::Type::UnsignedInt || type == Buffer::Type::UnsignedShort ||
                      type == Buffer::Type::Byte,
                  "Invalid type for IndexBuffer, only UnsignedInt, UnsignedShort "
                  "and Byte is premited");

        mCount = count;
        mUsage = usage;
        mType = type;
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {}

    void VulkanIndexBuffer::bind() const {}

    void VulkanIndexBuffer::unbind() const {}

    void VulkanIndexBuffer::updateData(void* data, uint32_t count, uint32_t offset) {
        UNUSED(data);
        UNUSED(count);
        UNUSED(offset);
    }

    Ref<IndexBuffer> IndexBuffer::Create(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        return createRef<VulkanIndexBuffer>(data, count, usage, type);
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        return createRef<VulkanIndexBuffer>(count, usage, type);
    }
} // namespace Car
