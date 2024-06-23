#include "Car/internal/OpenGL/Texture2D.hpp"


#include <stb/stb_image.h>
#include <glad/gl.h>


namespace Car { 
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, uint8_t* buffer, Format format, Format internalformat, Type type) {
        uint32_t opengl_type;
        switch (type) {
        case Texture2D::Type::Byte: opengl_type = GL_UNSIGNED_BYTE; break;
        default:
            CR_CORE_ERROR("Unrecognized Texture2D::Type: {}", (uint32_t)type);
            CR_EXIT(1);
        }
        uint32_t opengl_format;
        switch (format) {
        case Texture2D::Format::RGBA: opengl_format = GL_RGBA; mBPP = 4; break;
        default:
            CR_CORE_ERROR("Unrecognized Texture2D::Format for format param: {}", (uint32_t)format);
            CR_EXIT(1);
        }
        uint32_t opengl_internalformat;
        switch (internalformat) {
        case Texture2D::Format::RGBA8: opengl_internalformat = GL_RGBA8; break;
        default:
            CR_CORE_ERROR("Unrecognized Texture2D::Format for internalformat: {}", (uint32_t)internalformat);
            CR_EXIT(1);
        }

        mWidth = width;
        mHeight = height;

        if (!buffer) {
            CR_CORE_ERROR("Invalid buffer provided for Texture2D");
            CR_EXIT(1);
        }

        glGenTextures(1, &mID);
        glBindTexture(GL_TEXTURE_2D, mID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, opengl_internalformat, mWidth, mHeight, 0, opengl_format, opengl_type, buffer);
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath, bool flipped, Format format, Format internalformat, Type type) {
        uint32_t opengl_type;
        switch (type) {
        case Texture2D::Type::Byte: opengl_type = GL_UNSIGNED_BYTE; break;
        default:
            CR_CORE_ERROR("Unrecognized Texture2D::Type: {}", (uint32_t)type);
            CR_EXIT(1);
        }
        uint32_t opengl_format;
        switch (format) {
        case Texture2D::Format::RGBA: opengl_format = GL_RGBA; break;
        default:
            CR_CORE_ERROR("Unrecognized Texture2D::Format for format param: {}", (uint32_t)format);
            CR_EXIT(1);
        }
        uint32_t opengl_internalformat;
        switch (internalformat) {
        case Texture2D::Format::RGBA8: opengl_internalformat = GL_RGBA8; break;
        default:
            CR_CORE_ERROR("Unrecognized Texture2D::Format for internalformat: {}", (uint32_t)internalformat);
            CR_EXIT(1); 
        }

        stbi_set_flip_vertically_on_load(flipped ? 1 : 0);

        int width, height, bpp;
        uint8_t* imageData = stbi_load(filepath.c_str(), &width, &height, &bpp, 4);
        mWidth = width;
        mHeight = height;
        mBPP = bpp;

        if (!imageData) {
            CR_CORE_ERROR("Failed to load image: {0}", (std::string)stbi_failure_reason());
            CR_EXIT(1);
        }

        glGenTextures(1, &mID);
        glBindTexture(GL_TEXTURE_2D, mID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, opengl_internalformat, mWidth, mHeight, 0, opengl_format, opengl_type, imageData);

        stbi_image_free(imageData);
    } 

    OpenGLTexture2D::~OpenGLTexture2D() {
        glDeleteTextures(1, &mID);
    }

    void OpenGLTexture2D::updateData(uint8_t* buffer, Format internalFormat, Format format, Type type) {
        UNUSED(buffer);
        UNUSED(internalFormat);
        UNUSED(format);
        UNUSED(type);
    }

    void OpenGLTexture2D::updateData(const std::string& filepath, bool flipped, Format internalFormat, Format format, Type type) {
        UNUSED(filepath); 
        UNUSED(flipped);
        UNUSED(internalFormat);
        UNUSED(format);
        UNUSED(type);

    }

    void OpenGLTexture2D::bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, mID);
    }

    void OpenGLTexture2D::unbind() const {
        glBindTexture(GL_TEXTURE_2D, mID);
    }

    void OpenGLTexture2D::setFilters(int mag, int min) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    }   

    void OpenGLTexture2D::setRepeatX(bool v) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, v ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }

    void OpenGLTexture2D::setRepeatY(bool v) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, v ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }

    Ref<Texture2D> Texture2D::Create(const std::string& filepath, bool flipped, Format format, Format internalFormat, Type type) {
        return createRef<OpenGLTexture2D>(filepath, flipped, format, internalFormat, type);
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, uint8_t* buffer, Format format, Format internalFormat, Type type) {
        return createRef<OpenGLTexture2D>(width, height, buffer, internalFormat, format, type);
    }
}
