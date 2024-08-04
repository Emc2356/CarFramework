#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Geometry/Rect.hpp"
#include "Car/Renderer/Texture2D.hpp"

#ifndef CR_DEFAULT_CHARS
#define CR_DEFAULT_CHARS                                                                                               \
    " !\\\"#$%&'()*+,-./"                                                                                              \
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"                                                               \
    "abcdefghijklmnopqrstuvwxyz{|}~"
#endif // CR_DEFAULT_CHARS

namespace Car {
    class Renderer2D;

    class Font {
        struct Character {
            Rect rect;
            uint32_t advance;
        };

    public:
        Font(const std::string& path, uint32_t height, const std::string& charsToLoad = CR_DEFAULT_CHARS);
        ~Font();

        glm::ivec2 measureText(const std::string& text);
        Ref<Texture2D> getTexture() const { return mTexture; }

    private:
        Ref<Texture2D> mTexture;
        uint32_t mHeight;
        std::array<Character, 255> mCharacters;

        friend Renderer2D;
    };
} // namespace Car
