#include "Car/internal/OpenGL/VertexArray.hpp"

#include "Car/internal/OpenGL/VertexBuffer.hpp"

#include <glad/gl.h>
#include <memory>


namespace Car {
    OpenGLVertexArray::OpenGLVertexArray(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader) {
        glGenVertexArrays(1, &mID);

        mVb = vb;
        mIb = ib;
        mShader = shader;
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        glDeleteVertexArrays(1, &mID);
    }

    void OpenGLVertexArray::bind() const {
        glBindVertexArray(mID);
        mIb->bind();
        mVb->bind();
        ((OpenGLVertexBuffer*)(mVb.get()))->applyLayout();
        mShader->bind();
    }


    Ref<VertexArray> VertexArray::Create(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader) {
        // TODO: this
        auto ret = createRef<OpenGLVertexArray>(vb, ib, shader);
        return *(Ref<VertexArray>*)&ret;
    }
}
