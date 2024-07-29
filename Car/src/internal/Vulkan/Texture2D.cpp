#include "Car/internal/Vulkan/Texture2D.hpp"

#include <stb/stb_image.h>
#include <glad/vulkan.h>

namespace Car {
    VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height, uint8_t* buffer, Format format,
                                     Format internalformat, Type type) {
        UNUSED(buffer);

        mWidth = width;
        mHeight = height;
        mFormat = format;
        mInternalFormat = internalformat;
        mType = type;
    }

    VulkanTexture2D::VulkanTexture2D(const std::string& filepath, bool flipped, Format format, Format internalformat,
                                     Type type) {
        UNUSED(filepath);
        UNUSED(flipped);

        mWidth = 0;
        mHeight = 0;
        mFormat = format;
        mInternalFormat = internalformat;
        mType = type;
    }

    VulkanTexture2D::~VulkanTexture2D() {}

    void VulkanTexture2D::updateData(uint8_t* buffer, Format internalFormat, Format format, Type type) {
        UNUSED(buffer);
        UNUSED(internalFormat);
        UNUSED(format);
        UNUSED(type);
    }

    void VulkanTexture2D::updateData(const std::string& filepath, bool flipped, Format internalFormat, Format format,
                                     Type type) {
        UNUSED(filepath);
        UNUSED(flipped);
        UNUSED(internalFormat);
        UNUSED(format);
        UNUSED(type);
    }

    void VulkanTexture2D::bind(uint32_t slot) const { UNUSED(slot); }

    void VulkanTexture2D::unbind() const {}

    void VulkanTexture2D::setFilters(Filter minFilter, Filter magFilter) {
        UNUSED(minFilter);
        UNUSED(magFilter);
    }

    void VulkanTexture2D::setRepeatX(bool v) { UNUSED(v); }

    void VulkanTexture2D::setRepeatY(bool v) { UNUSED(v); }

    Ref<Texture2D> Texture2D::Create(const std::string& filepath, bool flipped, Format format, Format internalFormat,
                                     Type type) {
        return createRef<VulkanTexture2D>(filepath, flipped, format, internalFormat, type);
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, uint8_t* buffer, Format format,
                                     Format internalFormat, Type type) {
        return createRef<VulkanTexture2D>(width, height, buffer, format, internalFormat, type);
    }
} // namespace Car
