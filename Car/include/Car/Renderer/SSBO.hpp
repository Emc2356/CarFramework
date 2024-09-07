#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Buffer.hpp"

// TODO: Not implemented
namespace Car {
    class SSBO {
    public:
        virtual ~SSBO() = default;

        virtual void updateBuffer(void* data, uint64_t size) = 0;

        virtual uint64_t getSize() const = 0;
        virtual void bind(uint32_t binding) const = 0;
        virtual void unbind() const = 0;

        static Ref<SSBO> Create(void* data, uint64_t size, Car::Buffer::Usage usage = Car::Buffer::Usage::DynamicDraw);
    };
} // namespace Car
