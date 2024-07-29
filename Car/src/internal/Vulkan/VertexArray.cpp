#include "Car/internal/Vulkan/VertexArray.hpp"

#include "Car/internal/Vulkan/VertexBuffer.hpp"

#include <glad/vulkan.h>

namespace Car {
    VulkanVertexArray::VulkanVertexArray(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader) {
        mVb = reinterpretCastRef<VulkanVertexBuffer>(vb);
        mIb = reinterpretCastRef<VulkanIndexBuffer>(ib);
        mShader = reinterpretCastRef<VulkanShader>(shader);
        
        mShader->trueCreateImplementation(mVb);
    }

    VulkanVertexArray::~VulkanVertexArray() {}

    void VulkanVertexArray::bind() const {
        mShader->bind();
        mVb->bind();
        mIb->bind();
    }

    Ref<VertexArray> VertexArray::Create(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader) {
        return createRef<VulkanVertexArray>(vb, ib, shader);
    }
} // namespace Car
