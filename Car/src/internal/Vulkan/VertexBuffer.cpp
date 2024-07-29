#include "Car/internal/Vulkan/VertexBuffer.hpp"
#include "Car/Application.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <glad/vulkan.h>


uint32_t findMemoryType(uint32_t typeFilter, VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    throw std::runtime_error("failed to find suitable memory type!");
}


namespace Car {
    static VkFormat BufferLayoutDataTypeToVulkanType(BufferLayout::DataType type) {
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
            CR_CORE_ERROR("Unrecognized BufferLayout::DataType: {0}",
            static_cast<uint32_t>(type)); CR_DEBUGBREAK(); return VK_FORMAT_UNDEFINED;
        }
    }

    VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size, BufferLayout layout, Buffer::Usage usage) {
        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(Application::Get()->getWindow()->getGraphicsContext());
        mLayout = layout;
        mUsage = usage;
        mSize = size;
        mUsage = usage;

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
        
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = mSize;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkDevice device = mGraphicsContext->getDevice();
        VkPhysicalDevice physicalDevice = mGraphicsContext->getPhysicalDevice();
        
        if (vkCreateBuffer(device, &bufferInfo, nullptr, &mBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, mBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(
            memRequirements.memoryTypeBits, physicalDevice,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &mBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(device, mBuffer, mBufferMemory, 0);

        void* mappedData;
        vkMapMemory(device, mBufferMemory, 0, bufferInfo.size, 0, &mappedData);
            memcpy(mappedData, data, (size_t) bufferInfo.size);
        vkUnmapMemory(device, mBufferMemory);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        vkDeviceWaitIdle(mGraphicsContext->getDevice());
        vkDestroyBuffer(mGraphicsContext->getDevice(), mBuffer, nullptr);
        vkFreeMemory(mGraphicsContext->getDevice(), mBufferMemory, nullptr);
    }

    void VulkanVertexBuffer::bind() const {
        VkBuffer vertexBuffers[] = {mBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(mGraphicsContext->getCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);
    }

    void VulkanVertexBuffer::unbind() const {}

    void VulkanVertexBuffer::updateData(void* data, uint32_t size, uint32_t offset) {
        UNUSED(data);
        UNUSED(size);
        UNUSED(offset);
    }

    Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, BufferLayout layout, Buffer::Usage usage) {
        return createRef<VulkanVertexBuffer>(data, size, layout, usage);
    }
} // namespace Car
