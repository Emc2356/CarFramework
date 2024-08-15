#include "Car/internal/Vulkan/Texture2D.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <stb/stb_image.h>
#include <glad/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Car {
    VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height, void* pBuffer) {
        mWidth = 0;
        mHeight = 0;

        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());

        setInternalData(width, height, pBuffer);
    }

    VulkanTexture2D::VulkanTexture2D(const std::string& filepath, bool flipped) {
        mWidth = 0;
        mHeight = 0;

        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());

        stbi_set_flip_vertically_on_load(flipped);

        int texWidth, texHeight;
        void* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, nullptr, STBI_rgb_alpha);

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        setInternalData(texWidth, texHeight, pixels);

        stbi_image_free(pixels);
    }

    void VulkanTexture2D::setInternalData(uint32_t width, uint32_t height, void* pixels) {
        UNUSED(pixels);

        CR_ASSERT(width > 0 && height > 0, "width and height for Car::Texture2D must be bigger then 0");

        VkDeviceSize imageSize = width * height * 4;
        VkDevice device = mGraphicsContext->getDevice();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        mGraphicsContext->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       &stagingBuffer, &stagingBufferMemory);

        vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

        void* mappedData;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &mappedData);
        std::memcpy(mappedData, pixels, imageSize);
        vkUnmapMemory(device, stagingBufferMemory);

        if (mImage == VK_NULL_HANDLE || mImageMemory == VK_NULL_HANDLE || width != mWidth || height != mHeight) {
            if (mImage != VK_NULL_HANDLE) {
                vkDeviceWaitIdle(device);
                vkDestroyImage(device, mImage, nullptr);
                vkFreeMemory(device, mImageMemory, nullptr);
            }

            mGraphicsContext->createImage2D(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                                            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mImage, &mImageMemory);
        }
        vkBindImageMemory(device, mImage, mImageMemory, 0);

        mGraphicsContext->transitionImageLayout(&mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        mGraphicsContext->copyBufferToImage2D(&stagingBuffer, &mImage, width, height);
        // TODO: this is for samplers only
        mGraphicsContext->transitionImageLayout(&mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        mWidth = width;
        mHeight = height;

        mImageView = mGraphicsContext->createImageView(&mImage, VK_FORMAT_R8G8B8A8_SRGB);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    VulkanTexture2D::~VulkanTexture2D() {
        VkDevice device = mGraphicsContext->getDevice();

        vkDeviceWaitIdle(device);
        vkDestroyImage(device, mImage, nullptr);
        vkDestroyImageView(device, mImageView, nullptr);
        vkFreeMemory(device, mImageMemory, nullptr);
    }

    void VulkanTexture2D::updateData(const std::string& filepath, bool flipped) {
        mWidth = 0;
        mHeight = 0;

        stbi_set_flip_vertically_on_load(flipped);

        int texWidth, texHeight, texChannels;
        void* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        mWidth = texWidth;
        mHeight = texHeight;
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        UNUSED(imageSize);

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        stbi_image_free(pixels);
    }

    void VulkanTexture2D::bind(uint32_t slot) const { UNUSED(slot); }

    Ref<Texture2D> Texture2D::Create(const std::string& filepath, bool flipped) {
        return createRef<VulkanTexture2D>(filepath, flipped);
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, void* pBuffer) {
        return createRef<VulkanTexture2D>(width, height, pBuffer);
    }
} // namespace Car
