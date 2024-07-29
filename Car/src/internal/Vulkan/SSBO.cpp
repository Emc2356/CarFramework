#include "Car/internal/Vulkan/SSBO.hpp"

#include <glad/vulkan.h>

namespace Car {
    VulkanSSBO::VulkanSSBO(void* data, uint32_t size, Buffer::Usage usage) {
        UNUSED(data);
        UNUSED(usage);
        mSize = size;
    }

    VulkanSSBO::~VulkanSSBO() {}

    void VulkanSSBO::updateBuffer(void* data, uint32_t size) {
        UNUSED(data);
        UNUSED(size);
    }

    void VulkanSSBO::bind(uint32_t binding) const { UNUSED(binding); }

    void VulkanSSBO::unbind() const {}

    Ref<SSBO> SSBO::Create(void* data, uint32_t size, Buffer::Usage usage) {
        return createRef<VulkanSSBO>(data, size, usage);
    }
} // namespace Car
