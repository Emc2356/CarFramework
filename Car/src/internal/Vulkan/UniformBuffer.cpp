#include "Car/internal/Vulkan/UniformBuffer.hpp"
#include "Car/Core/Log.hpp"

#include <glad/vulkan.h>

namespace Car {
    VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding, Buffer::Usage usage) {
        mSize = size;
        mBinding = binding;
        mUsage = usage;
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {}

    void VulkanUniformBuffer::setData(const void* data, uint32_t offset /*=0*/) {
        UNUSED(data);
        UNUSED(offset);
    }

    Ref<Car::UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding, Buffer::Usage usage) {
        return createRef<VulkanUniformBuffer>(size, binding, usage);
    }
} // namespace Car
