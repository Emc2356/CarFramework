#include "Car/Renderer/Font.hpp"

#include <stb/stb_truetype.h>


namespace Car {
    Font::Font(const std::string& path, uint32_t size) {
        UNUSED(path);
        UNUSED(size);
        UNUSED(mFontInfo);
        // int res = stbtt_InitFont(&mFontInfo, const unsigned char *data, int offset);
    }
    }
