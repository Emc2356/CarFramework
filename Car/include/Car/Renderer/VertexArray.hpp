#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/VertexBuffer.hpp"
#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/Shader.hpp"

namespace Car {
    class VertexArray {
    public:
        virtual ~VertexArray() = default;

        // TODO: Delete this as it doesnt make sense to change what parts of this exists
        virtual void setVertexBuffer(Ref<VertexBuffer> vb) = 0;
        virtual void setIndexBuffer(Ref<IndexBuffer> ib) = 0;
        virtual void setShader(Ref<Shader> shader) = 0;

        virtual Ref<VertexBuffer> getVertexBuffer() const = 0;
        virtual Ref<IndexBuffer> getIndexBuffer() const = 0;
        virtual Ref<Shader> getShader() const = 0;

        virtual void bind() const = 0;

        static Ref<VertexArray> Create(Ref<VertexBuffer> vb, Ref<IndexBuffer> ib, Ref<Shader> shader);
    };
} // namespace Car
