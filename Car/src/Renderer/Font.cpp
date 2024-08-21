#include "Car/Renderer/Font.hpp"
#include "Car/Renderer/Texture2D.hpp"

#include <cstdint>
#include <cstring>

#include <ft2build.h>
#include <freetype/freetype.h>

// no questions asked
#define heightPadding 20

namespace Car {
    static bool sFreeTypeInitialized = false;
    static FT_Library sFt;

    Font::Font(const std::string& path, uint32_t height, const std::string& charsToLoad) {
        mHeight = height;
        mCharacters.resize(255);
        if (!sFreeTypeInitialized) {
            CR_VERIFYN(FT_Init_FreeType(&sFt), "ERROR::FREETYPE: Could not init FreeType Library");
            sFreeTypeInitialized = true;
        }

        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("`" + path + "` doesnt exist");
        }

        FT_Face face;
        CR_VERIFYN(FT_New_Face(sFt, path.c_str(), 0, &face), "ERROR::FREETYPE: Failed to load font");

        FT_Set_Pixel_Sizes(face, 0, height);

        uint32_t textureWidth = 0;
        uint32_t textureHeight = 0;
        uint32_t highestYBearing = 0;

        // first pass to figure out the texture's width
        for (uint32_t i = 0; i < charsToLoad.size(); i++) {
            char chr = charsToLoad[i];
            CR_VERIFYN(FT_Load_Char(face, chr, FT_LOAD_RENDER), "ERROR::FREETYTPE: Failed to load Glyp");

            textureWidth += face->glyph->bitmap.width;
            textureHeight = MAX(textureHeight, face->glyph->bitmap.rows);
            highestYBearing = MAX(highestYBearing, (uint32_t)face->glyph->bitmap_top);
        }

        uint8_t* pixelBuffer = (uint8_t*)malloc(textureWidth * (textureHeight + heightPadding) * sizeof(uint8_t) * 4);
        memset(pixelBuffer, 0, textureWidth * (textureHeight + heightPadding) * sizeof(uint8_t) * 4);

        // second pass to get the bitmap data
        uint32_t curAdvance = 0;
        for (uint32_t i = 0; i < charsToLoad.size(); i++) {
            const char chr = charsToLoad[i];
            CR_VERIFYN(FT_Load_Char(face, chr, FT_LOAD_RENDER), "ERROR::FREETYTPE: Failed to load Glyp");

            uint32_t ystart = highestYBearing - face->glyph->bitmap_top;

            for (uint32_t x = 0; x < face->glyph->bitmap.width; x++) {
                for (uint32_t y = 0; y < face->glyph->bitmap.rows; y++) {
                    int cond = face->glyph->bitmap.buffer[x + y * face->glyph->bitmap.width] != 0;
                    uint32_t posInBuf = ((x + curAdvance) + ((ystart + y) * textureWidth)) * 4;
                    pixelBuffer[posInBuf + 0] = 255;
                    pixelBuffer[posInBuf + 1] = 255;
                    pixelBuffer[posInBuf + 2] = 255;
                    pixelBuffer[posInBuf + 3] = 255 * cond;
                }
            }
            mCharacters[chr].rect = {(float)curAdvance, (float)0, (float)face->glyph->bitmap.width, (float)mHeight};
            mCharacters[chr].advance = (uint32_t)face->glyph->advance.x >> 6;

            curAdvance += face->glyph->bitmap.width;
        }

        mTexture = Texture2D::Create(textureWidth, textureHeight + heightPadding, pixelBuffer);

        free(pixelBuffer);
    }

    glm::ivec2 Font::measureText(const std::string& text) {
        uint32_t width = 0;
        uint32_t maxWidth = 0;
        uint32_t height = mHeight;

        for (size_t i = 0; i < text.size(); i++) {
            const uint8_t chr = text[i];
            if (chr == '\n') {
                // slight padding to stop the font looking weird
                // incase a glyph has the same height as the font height
                height += mHeight + 1;
                maxWidth = MAX(maxWidth, width);
                width = 0;
                continue;
            }

            const Font::Character& character = mCharacters[chr];

            width += character.advance;
        }

        maxWidth = MAX(maxWidth, width);

        return glm::ivec2(maxWidth, height);
    }

    Font::~Font() {}
} // namespace Car
