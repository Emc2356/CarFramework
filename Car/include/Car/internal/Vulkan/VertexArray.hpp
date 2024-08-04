#pragma once

#include "Car/Renderer/VertexArray.hpp"

#include "Car/internal/Vulkan/Shader.hpp"
#include "Car/internal/Vulkan/IndexBuffer.hpp"
#include "Car/internal/Vulkan/VertexBuffer.hpp"

namespace Car {
    class VulkanVertexArray : public VertexArray {
    public:
        VulkanVertexArray(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader);
        virtual ~VulkanVertexArray() override;

        virtual void setVertexBuffer(Ref<VertexBuffer> vb) override {
            mVb = reinterpretCastRef<VulkanVertexBuffer>(vb);
        }
        virtual void setIndexBuffer(Ref<IndexBuffer> ib) override { mIb = reinterpretCastRef<VulkanIndexBuffer>(ib); }
        virtual void setShader(Ref<Shader> shader) override { mShader = reinterpretCastRef<VulkanShader>(shader); }

        virtual Ref<VertexBuffer> getVertexBuffer() const override { return mVb; }
        virtual Ref<IndexBuffer> getIndexBuffer() const override { return mIb; }
        virtual Ref<Shader> getShader() const override { return mShader; }

        virtual void bind() const override;

    private:
        Ref<VulkanVertexBuffer> mVb;
        Ref<VulkanIndexBuffer> mIb;
        Ref<VulkanShader> mShader;
    };
} // namespace Car
