#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/VertexBuffer.hpp"
#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/Shader.hpp"


namespace Car {
    class VertexArray {
    public:
        virtual ~VertexArray() = default;

        void setVertexBuffer(Ref<VertexBuffer> vb) { mVb = vb; }
        void setIndexBuffer(Ref<IndexBuffer> ib) { mIb = ib; }
        void setShader(Ref<Shader> shader) { mShader = shader; }

        Ref<VertexBuffer> getVertexBuffer() const { return mVb; }
        Ref<IndexBuffer> getIndexBuffer() const { return mIb; }
        Ref<Shader> getShader() const { return mShader; }

        virtual void bind() const = 0;

        static Ref<VertexArray> Create(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader);
    protected:
        Ref<VertexBuffer> mVb;
        Ref<IndexBuffer> mIb;
        Ref<Shader> mShader;
    };
}
