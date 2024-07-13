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
        enum class Filter {
            None,
            Nearest,
            Linear,
        };
    public:
        virtual ~Texture2D() = default;
        virtual void updateData(uint8_t* buffer, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte) = 0;
        virtual void updateData(const std::string& filepath, bool flipped=false, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte) = 0;

        virtual void bind(uint32_t slot=0) const = 0;
        virtual void unbind() const = 0;

        virtual void setFilters(Filter minFilter, Filter magFilter) = 0;
        virtual void setRepeatX(bool v) = 0;
        virtual void setRepeatY(bool v) = 0;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual uint32_t getBPP() const = 0;
        
        virtual bool operator==(Ref<Texture2D> other) const = 0;
        virtual bool operator!=(Ref<Texture2D> other) const = 0;

        static Ref<Texture2D> Create(const std::string& filepath, bool flipped=false, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height, uint8_t* buffer=nullptr, Format format=Format::RGBA, Format internalFormat=Format::RGBA8, Type type=Type::Byte);
    };
}
