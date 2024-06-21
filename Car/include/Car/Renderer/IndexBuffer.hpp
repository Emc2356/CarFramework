#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Buffer.hpp"


namespace Car {
    class IndexBuffer {
    public:
        virtual ~IndexBuffer() = default;

        virtual void bind() const = 0;
        // for debug purposes
        virtual void unbind() const = 0;

        // count and offset are in elements and not in bytes
        virtual void updateData(void* data, uint32_t count, uint32_t offset) = 0;

        uint32_t getCount() const { return mCount; }
        Buffer::Usage getUsage() const { return mUsage; }
        Buffer::Type getType() const { return mType; }
        // not guaranteed that the pointer is alive
        void* getData() const { return mData; }

        // IndexBuffer doesnt become owner of the data but holds a reference for debug purposes only
        static Ref<IndexBuffer> Create(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type);
        static Ref<IndexBuffer> Create(uint32_t count, Buffer::Usage usage, Buffer::Type type);
    protected:
        uint32_t mCount;
        void* mData;
        Buffer::Usage mUsage;
        Buffer::Type mType;
    };
}
