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

        virtual uint32_t getCount() const = 0;
        virtual Buffer::Usage getUsage() const = 0;
        virtual Buffer::Type getType() const = 0;

        static Ref<IndexBuffer> Create(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type);
        static Ref<IndexBuffer> Create(uint32_t count, Buffer::Usage usage, Buffer::Type type);
    };
} // namespace Car
