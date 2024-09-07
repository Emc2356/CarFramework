#pragma once

#include "Car/Renderer/SSBO.hpp"

namespace Car {
    class VulkanSSBO : public SSBO {
    public:
        VulkanSSBO(void* data, uint64_t size, Buffer::Usage usage);
        virtual ~VulkanSSBO() override;

        virtual void updateBuffer(void* data, uint64_t size) override;

        virtual uint64_t getSize() const override { return mSize; }
        virtual void bind(uint32_t binding) const override;
        virtual void unbind() const override;

    private:
        uint64_t mSize;
    };
} // namespace Car
