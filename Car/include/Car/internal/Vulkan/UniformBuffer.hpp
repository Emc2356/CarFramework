#pragma once

#include "Car/Renderer/UniformBuffer.hpp"

namespace Car {
    class VulkanUniformBuffer : public UniformBuffer {
    public:
        VulkanUniformBuffer(uint32_t size, uint32_t binding, Buffer::Usage usage);
        ~VulkanUniformBuffer();

        virtual uint32_t getSize() const override { return mSize; }
        virtual uint32_t getBinding() const override { return mBinding; }
        virtual Buffer::Usage getUsage() const override { return mUsage; }

        virtual void setData(const void* data, uint32_t offset = 0) override;

    private:
        uint32_t mSize;
        uint32_t mBinding;
        Buffer::Usage mUsage;
    };
} // namespace Car
