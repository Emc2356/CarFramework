#pragma once

#include "Car/Renderer/VertexArray.hpp"


namespace Car {
    class OpenGLVertexArray : public VertexArray {
    public:
        OpenGLVertexArray(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader);
        virtual ~OpenGLVertexArray() override;

        virtual void setVertexBuffer(Ref<VertexBuffer> vb) override { mVb = vb; }
        virtual void setIndexBuffer(Ref<IndexBuffer> ib) override { mIb = ib; }
        virtual void setShader(Ref<Shader> shader) override { mShader = shader; }

        virtual Ref<VertexBuffer> getVertexBuffer() const override { return mVb; }
        virtual Ref<IndexBuffer> getIndexBuffer() const override { return mIb; }
        virtual Ref<Shader> getShader() const override { return mShader; }
        
        virtual void bind() const override;
    private:
        Ref<VertexBuffer> mVb;
        Ref<IndexBuffer> mIb;
        Ref<Shader> mShader;
        
        uint32_t mID;
    };
}
