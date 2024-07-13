#pragma once

#include "Car/Core/Core.hpp"
#include <stb/stb_truetype.h>


namespace Car {
    class Font {
    public:
        Font(const std::string& path, uint32_t size);
    private:
        stbtt_fontinfo mFontInfo;
    };
}
