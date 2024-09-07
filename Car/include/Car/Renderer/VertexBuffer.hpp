#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Buffer.hpp"

namespace Car {
    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;

        virtual void bind() const = 0;

        virtual void updateData(void* data, uint64_t size, uint64_t offset) = 0;

        virtual uint64_t getSize() const = 0;
        virtual Buffer::Usage getUsage() const = 0;

        static Ref<VertexBuffer> Create(void* data, uint64_t size, Buffer::Usage usage);
    };
} // namespace Car
