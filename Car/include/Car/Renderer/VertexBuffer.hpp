#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Buffer.hpp"
#include "Car/Renderer/BufferLayout.hpp"


namespace Car {
    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;

        virtual void bind() const = 0;
        // for debug purposes
        virtual void unbind() const = 0;

        // count and offset are in elements and not in bytes
        virtual void updateData(void* data, uint32_t count, uint32_t offset) = 0;

        virtual void setLayout(const BufferLayout& layout) = 0;
        virtual const BufferLayout& getLayout() { return mLayout; }

        uint32_t getCount() const { return mCount; }
        Buffer::Usage getUsage() const { return mUsage; }
        Buffer::Type getType() const { return mType; }
        // not guaranteed that the pointer is alive
        void* getData() const { return mData; }

        // VertexBuffer doesnt become owner of the data but holds a reference for debug purposes only
        static Ref<VertexBuffer> Create(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type);
        static Ref<VertexBuffer> Create(uint32_t count, Buffer::Usage usage, Buffer::Type type);
    protected:
        BufferLayout mLayout;
        uint32_t mCount;
        void* mData;
        Buffer::Usage mUsage;
        Buffer::Type mType;
    };
}
