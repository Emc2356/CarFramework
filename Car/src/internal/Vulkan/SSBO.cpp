#include "Car/internal/Vulkan/SSBO.hpp"

#include <glad/vulkan.h>
#include <stdexcept>

namespace Car {
    VulkanSSBO::VulkanSSBO(void* data, uint64_t size, Buffer::Usage usage) {
        UNUSED(data);
        UNUSED(usage);
        mSize = size;
        
        throw std::runtime_error("Vulkan SSBO not implemented yet");
    }

    VulkanSSBO::~VulkanSSBO() {}

    void VulkanSSBO::updateBuffer(void* data, uint64_t size) {
        UNUSED(data);
        UNUSED(size);
    }

    void VulkanSSBO::bind(uint32_t binding) const { UNUSED(binding); }

    void VulkanSSBO::unbind() const {}

    Ref<SSBO> SSBO::Create(void* data, uint64_t size, Buffer::Usage usage) {
        return createRef<VulkanSSBO>(data, size, usage);
    }
} // namespace Car
