#pragma once

#include "Car/Renderer/VertexArray.hpp"


namespace Car {
    class OpenGLVertexArray : public VertexArray {
    public:
        OpenGLVertexArray(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader);
        virtual ~OpenGLVertexArray() override;

        virtual void bind() const override;
    private:
        uint32_t mID;
    };
}
