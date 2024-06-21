#pragma once

#include "Car/Core/Core.hpp"


namespace Car {
    class Texture2D {
    public:
        enum class Format {
            None,
            RGBA8,
            RGBA,
        };
        enum class Type {
            None,
            Byte,
        };
    public:
        virtual ~Texture2D() = default;
        virtual void updateData(uint8_t* buffer, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte) = 0;
        virtual void updateData(const std::string& filepath, bool flipped=false, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte) = 0;

        virtual void bind(uint32_t slot=0) const = 0;
        virtual void unbind() const = 0;

        virtual void setFilters(int min, int mag) = 0;
        virtual void setRepeatX(bool v) = 0;
        virtual void setRepeatY(bool v) = 0;

        uint32_t getWidth() const { return mWidth; }
        uint32_t getHeight() const { return mHeight; }
        uint32_t getBPP() const { return mBPP; }

        static Ref<Texture2D> Create(const std::string& filepath, bool flipped=false, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height, uint8_t* buffer=nullptr, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte);
    public:
        uint32_t mWidth;
        uint32_t mHeight;
        // bits per pixel
        uint32_t mBPP;
    };
}
