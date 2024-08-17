#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Geometry/Geometry.hpp"

namespace Car {
    class Texture2D {
    public:
        enum class Filter {
            None,
            Nearest,
            Linear,
        };

    public:
        virtual ~Texture2D() = default;
        virtual void updateData(const std::string& filepath, bool flipped = false) = 0;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;

        virtual Rect getRect() const = 0;

        virtual bool operator==(Ref<Texture2D> other) const = 0;
        virtual bool operator!=(Ref<Texture2D> other) const = 0;

        static Ref<Texture2D> Create(const std::string& filepath, bool flipped = false);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height, void* pBuffer);
    };
} // namespace Car
