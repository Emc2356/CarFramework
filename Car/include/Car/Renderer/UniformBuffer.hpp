#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Buffer.hpp"

namespace Car {
    class UniformBuffer {
    public:
        virtual ~UniformBuffer() = default;

        virtual void setData(const void* data) = 0;

        virtual uint32_t getSize() const = 0;
        virtual uint32_t getBinding() const = 0;
        virtual Buffer::Usage getUsage() const = 0;

        // in vulkan the usage wont matter
        static Ref<Car::UniformBuffer> Create(uint32_t size, uint32_t binding,
                                              Buffer::Usage usage = Buffer::Usage::DynamicDraw);
    };
} // namespace Car
