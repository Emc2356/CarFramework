#pragma once

#include "Car/Renderer/Texture2D.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

namespace Car {
    class VulkanTexture2D : public Texture2D {
    public:
        VulkanTexture2D(uint32_t width, uint32_t height, void* pBuffer);
        VulkanTexture2D(const std::string& filepath, bool flipped);
        
        void createTextureImage2D(void* pBuffer);
        void createImageView();
        void createImageSampler();

        virtual ~VulkanTexture2D() override;

        virtual void updateData(const std::string& filepath, bool flipped = false) override;

        virtual void bind(uint32_t slot = 0) const override;

        virtual uint32_t getWidth() const override { return mWidth; }
        virtual uint32_t getHeight() const override { return mHeight; }

        virtual Rect getRect() const override { return {0.0f, 0.0f, (float)mWidth, (float)mHeight}; }

        virtual bool operator==(Ref<Texture2D> other) const override {
            return static_cast<const void*>(this) == static_cast<const void*>(other.get());
        }
        virtual bool operator!=(Ref<Texture2D> other) const override {
            return static_cast<const void*>(this) != static_cast<const void*>(other.get());
        }
        
        VkDescriptorImageInfo getDescriptorImageInfo() {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = mImageView;
            imageInfo.sampler = mSampler;

            return imageInfo;
        }

    private:
        void setInternalData(uint32_t width, uint32_t height, void* pixels);

    private:
        uint32_t mWidth;
        uint32_t mHeight;

        Ref<VulkanGraphicsContext> mGraphicsContext;

        VkImage mImage;
        VkDeviceMemory mImageMemory;
        VkImageView mImageView;
        VkSampler mSampler;
    };
} // namespace Car
