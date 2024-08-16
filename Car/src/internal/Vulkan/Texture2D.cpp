#include "Car/internal/Vulkan/Texture2D.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <stb/stb_image.h>
#include <glad/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Car {
    VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height, void* pBuffer) {
        mWidth = width;
        mHeight = height;

        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());

        createTextureImage2D(pBuffer);
        createImageView();
        createImageSampler();        
    }

    VulkanTexture2D::VulkanTexture2D(const std::string& filepath, bool flipped) {
        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());

        stbi_set_flip_vertically_on_load(flipped);

        int texWidth, texHeight;
        void* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, nullptr, STBI_rgb_alpha);

        mWidth = texWidth;
        mHeight = texHeight;
        
        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        createTextureImage2D(pixels);
        createImageView();
        createImageSampler();       

        stbi_image_free(pixels);
    }

    void VulkanTexture2D::createTextureImage2D(void* pBuffer) {
        VkDeviceSize imageSize = mWidth * mHeight * 4;
        VkDevice device = mGraphicsContext->getDevice();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        mGraphicsContext->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       &stagingBuffer, &stagingBufferMemory);

        vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

        void* mappedData;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &mappedData);
        std::memcpy(mappedData, pBuffer, imageSize);
        vkUnmapMemory(device, stagingBufferMemory);

        mGraphicsContext->createImage2D(mWidth, mHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mImage, &mImageMemory);
        
        vkBindImageMemory(device, mImage, mImageMemory, 0);

        mGraphicsContext->transitionImageLayout(&mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        mGraphicsContext->copyBufferToImage2D(&stagingBuffer, &mImage, mWidth, mHeight);
        // TODO: this is for samplers only
        mGraphicsContext->transitionImageLayout(&mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }
    
    void VulkanTexture2D::createImageView() {
        mImageView = mGraphicsContext->createImageView(&mImage, VK_FORMAT_R8G8B8A8_SRGB);
    }
    
    void VulkanTexture2D::createImageSampler() {
        // TODO: TextureSpecification
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = mGraphicsContext->getPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (vkCreateSampler(mGraphicsContext->getDevice(), &samplerInfo, nullptr, &mSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
    
    VulkanTexture2D::~VulkanTexture2D() {
        VkDevice device = mGraphicsContext->getDevice();

        vkDeviceWaitIdle(device);
        vkDestroySampler(device, mSampler, nullptr);
        vkDestroyImageView(device, mImageView, nullptr);
        vkDestroyImage(device, mImage, nullptr);
        vkFreeMemory(device, mImageMemory, nullptr);
    }

    void VulkanTexture2D::updateData(const std::string& filepath, bool flipped) {
        UNUSED(filepath);
        UNUSED(flipped);
    }

    void VulkanTexture2D::bind(uint32_t slot) const { UNUSED(slot); }

    Ref<Texture2D> Texture2D::Create(const std::string& filepath, bool flipped) {
        return createRef<VulkanTexture2D>(filepath, flipped);
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, void* pBuffer) {
        return createRef<VulkanTexture2D>(width, height, pBuffer);
    }
} // namespace Car
