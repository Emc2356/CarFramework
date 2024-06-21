#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Buffer.hpp"


namespace Car {
    class UniformBuffer {
    public:
        virtual ~UniformBuffer() = default;

        virtual void setData(const void* data, uint32_t offset=0) = 0;

        uint32_t getSize() const { return mSize; }
        uint32_t getBinding() const { return mBinding; }
        Buffer::Usage getUsage() const { return mUsage; }

        static Ref<Car::UniformBuffer> Create(uint32_t size, uint32_t binding, Buffer::Usage usage);  
    protected:
        uint32_t mSize;
        uint32_t mBinding;
        Buffer::Usage mUsage;
    };
}
