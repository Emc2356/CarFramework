#pragma once

#include "Car/Renderer/Texture2D.hpp"

namespace Car {
    class OpenGLTexture2D : public Texture2D {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height, uint8_t* buffer, Format format, Format internalformat,
                        Type type);
        OpenGLTexture2D(const std::string& filepath, bool flipped, Format format, Format internalformat, Type type);

        virtual ~OpenGLTexture2D() override;

        virtual void updateData(uint8_t* buffer, Format format = Format::RGBA, Format internalFormat = Format::RGBA8,
                                Type type = Type::Byte) override;
        virtual void updateData(const std::string& filepath, bool flipped = false, Format format = Format::RGBA,
                                Format internalFormat = Format::RGBA8, Type type = Type::Byte) override;

        virtual void bind(uint32_t slot = 0) const override;
        virtual void unbind() const override;

        virtual uint32_t getWidth() const override { return mWidth; }
        virtual uint32_t getHeight() const override { return mHeight; }
        virtual uint32_t getBPP() const override { return mBPP; }

        virtual Rect getRect() const override { return {0, 0, (int32_t)mWidth, (int32_t)mHeight}; }

        virtual bool operator==(Ref<Texture2D> other) const override {
            return mID == ((OpenGLTexture2D*)other.get())->mID;
        }
        virtual bool operator!=(Ref<Texture2D> other) const override {
            return mID != ((OpenGLTexture2D*)other.get())->mID;
        }

        virtual void setFilters(Filter minFilter, Filter magFilter) override;
        virtual void setRepeatX(bool v) override;
        virtual void setRepeatY(bool v) override;

    private:
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mBPP;
        uint32_t mID;
        uint32_t mInternalFormatOpenGL;
        uint32_t mFormatOpenGL;
        uint32_t mTypeOpenGL;
    };
} // namespace Car
