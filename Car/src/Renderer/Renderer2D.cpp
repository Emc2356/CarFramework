#include "Car/Renderer/Renderer.hpp"
#include "Car/Renderer/Renderer2D.hpp"
#include "Car/Application.hpp"
#include "Car/Core/Core.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Renderer/Buffer.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/Renderer/VertexArray.hpp"
#include "Car/Renderer/VertexBuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

struct Renderer2DData {
    Car::Ref<Car::Shader> texturesShader;
    Car::Ref<Car::UniformBuffer> texturesUBO;
    Car::Ref<Car::IndexBuffer> texturesIB;
    Car::Ref<Car::VertexBuffer> texturesVB;
    Car::Ref<Car::VertexArray> texturesVA;
    Car::Ref<Car::Texture2D> whiteTexture;

    uint32_t texturesMaxBatchSize;
    uint32_t texturesCurrentBatchSize;
    float* texturesVertexBufferData;
    uint32_t* texturesIndexBufferData;

    std::vector<Car::Ref<Car::Texture2D>> textureTextures;
};

namespace Car {
    static Renderer2DData* sData = nullptr;

    void Renderer2D::Init() {
        sData = new Renderer2DData();
        // internal use only so no reason to register with the ResourceManager
        sData->texturesShader = Shader::Create("builtin/texture.vert", "builtin/texture.frag");
        sData->texturesUBO = UniformBuffer::Create(sizeof(glm::mat4), 0, Buffer::Usage::DynamicDraw);

        // TODO: change the batch size so the index buffer can use uint32_t
        sData->texturesMaxBatchSize = 20000;
        // sData->texturesMaxBatchSize = 10800;
        sData->texturesCurrentBatchSize = 0;

        BufferLayout layout = {
            {"iPos", BufferLayout::DataType::Float2},
            {"iSourceUV", BufferLayout::DataType::Float2},
            {"iTint", BufferLayout::DataType::Float3},
            {"iTextureID", BufferLayout::DataType::Float},
        };

        sData->texturesVertexBufferData = (float*)malloc(sData->texturesMaxBatchSize * 4 * layout.getTotalSize());
        sData->texturesIndexBufferData = (uint32_t*)malloc(sData->texturesMaxBatchSize * 6 * sizeof(uint32_t));

        // initialize the index buffer since that will be the same always
        for (size_t i = 0; i < sData->texturesMaxBatchSize; i++) {
            sData->texturesIndexBufferData[i * 6 + 0] = i * 4 + 0;
            sData->texturesIndexBufferData[i * 6 + 1] = i * 4 + 1;
            sData->texturesIndexBufferData[i * 6 + 2] = i * 4 + 2;
            sData->texturesIndexBufferData[i * 6 + 3] = i * 4 + 2;
            sData->texturesIndexBufferData[i * 6 + 4] = i * 4 + 3;
            sData->texturesIndexBufferData[i * 6 + 5] = i * 4 + 0;
        }
        // 0b1_00111000_01111111
        sData->texturesIB = IndexBuffer::Create((void*)sData->texturesIndexBufferData, sData->texturesMaxBatchSize * 6,
                                                Buffer::Usage::StaticDraw, Buffer::Type::UnsignedInt);

        sData->texturesVB = VertexBuffer::Create(nullptr, sData->texturesMaxBatchSize * 4 * layout.getTotalSize(),
                                                 layout, Buffer::Usage::DynamicDraw);

        sData->texturesVA = VertexArray::Create(sData->texturesVB, sData->texturesIB, sData->texturesShader);

        uint32_t whiteTextureData = 0xFFFFFFFF;
        sData->whiteTexture = Texture2D::Create(1, 1, (uint8_t*)&whiteTextureData);
    }

    void Renderer2D::Shutdown() {
        free(sData->texturesVertexBufferData);
        free(sData->texturesIndexBufferData);
        delete sData;
    }

    void Renderer2D::Begin() {
        if (sData->texturesCurrentBatchSize > 0) {
            CR_CORE_ERROR("Called Car::Renderer2D::Begin() without closing the last begin");
        }
    }

    void Renderer2D::End() { Renderer2D::FlushTextures(); }

    void Renderer2D::FlushTextures() {
        // no work to be done, early return
        if (sData->texturesCurrentBatchSize == 0) {
            return;
        }

        uint32_t width = Application::Get()->getWindow()->getWidth();
        uint32_t height = Application::Get()->getWindow()->getHeight();

        glm::mat4 proj = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

        sData->texturesUBO->setData(glm::value_ptr(proj));

        for (size_t i = 0; i < sData->textureTextures.size(); i++) {
            sData->textureTextures[i]->bind(i);
        }

        sData->texturesVB->updateData(
            (void*)sData->texturesVertexBufferData,
            sData->texturesCurrentBatchSize * 4 * sData->texturesVB->getLayout().getTotalSize(), 0);

        Renderer::DrawTriangles(sData->texturesVA, sData->texturesCurrentBatchSize * 2);

        sData->textureTextures.clear();
        sData->texturesCurrentBatchSize = 0;
    }

    // TODO: This is weirdly slow
    int8_t Renderer2D::getTextureID(const Ref<Texture2D>& texture) {
        for (size_t i = 0; i < sData->textureTextures.size(); i++) {
            // small trick to save some performace here and there for the vtable
            // lookup
            if ((void*)sData->textureTextures[i].get() == (void*)texture.get()) {
                return i;
            }
        }
        if (sData->textureTextures.size() >= 16) {
            // invalidate the data to make sure this is resolved
            sData->texturesCurrentBatchSize = 0;
            return -1;
        }
        sData->textureTextures.push_back(texture);
        return sData->textureTextures.size() - 1;
    }

    void Renderer2D::DrawTexture(const Ref<Texture2D>& texture, const Rect& dest, const glm::vec3& tint) {
        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawTexture too many textures sent, maximum as "
                          "16, call Car::Renderer2D::FlushTextures if you plan on using "
                          "more than 16 textures");
            return;
        }

        Renderer2D::DrawTextureFromID(dest, textureID, tint);
    }

    void Renderer2D::DrawTexture(const Ref<Texture2D>& texture, const glm::ivec2& pos, const glm::vec3& tint) {
        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawTexture too many textures sent, maximum as "
                          "16, call Car::Renderer2D::FlushTextures if you plan on using "
                          "more than 16 textures");
            return;
        }

        Renderer2D::DrawTextureFromID({pos.x, pos.y, (int32_t)texture->getWidth(), (int32_t)texture->getHeight()},
                                      textureID, tint);
    }

    void Renderer2D::DrawSubTexture(const Ref<Texture2D>& texture, const Rect& source, const Rect& dest,
                                    const glm::vec3& tint) {
        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawSubTexture too many textures sent, maximum "
                          "as 16, call Car::Renderer2D::FlushTextures if you plan on using "
                          "more than 16 textures");
            return;
        }

        Renderer2D::DrawSubTextureFromID(texture, source, dest, textureID, tint);
    }

    void Renderer2D::DrawSubTexture(const Ref<Texture2D>& texture, const Rect& source, const glm::ivec2& pos,
                                    const glm::vec3& tint) {
        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawSubTexture too many textures sent, maximum "
                          "as 16, call Car::Renderer2D::FlushTextures if you plan on using "
                          "more than 16 textures");
            return;
        }

        Renderer2D::DrawSubTextureFromID(texture, source, {pos.x, pos.y, source.w - source.x, source.h - source.w},
                                         textureID, tint);
    }

    void Renderer2D::DrawRect(const Rect& rect, const glm::vec3& color) {
        int8_t textureID = getTextureID(sData->whiteTexture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawRect too many textures sent, maximum as 16, "
                          "call Car::Renderer2D::FlushTextures if you plan on using more "
                          "than 16 textures");
            return;
        }

        Renderer2D::DrawTextureFromID(rect, textureID, color);
    }

    void Renderer2D::DrawText(const Ref<Font>& font, const std::string& text, const glm::ivec2& pos,
                              const glm::vec3& color) {
        Ref<Texture2D> texture = font->getTexture();

        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawText too many textures sent, maximum as 16, "
                          "call Car::Renderer2D::FlushTextures if you plan on using more "
                          "than 16 textures");
            return;
        }

        int32_t fontHeight = font->mHeight;

        uint32_t x = pos.x;
        uint32_t y = pos.y;

        for (size_t i = 0; i < text.size(); i++) {
            const uint8_t chr = text[i];
            if (chr == '\n') {
                // slight padding to stop the font looking weird
                // incase a glyph has the same height as the font height
                y += fontHeight + 1;
                x = pos.x;
                continue;
            }

            const Font::Character& character = font->mCharacters[chr];

            Renderer2D::DrawSubTextureFromID(texture, character.rect,
                                             {(int32_t)x, (int32_t)y, character.rect.w, fontHeight}, textureID, color);

            x += character.advance;
        }
    }

    void Renderer2D::DrawSubTextureFromID(const Ref<Texture2D>& texture, const Rect& source, const Rect& dest,
                                          int8_t textureID, const glm::vec3& tint) {
        UNUSED(tint);
        uint32_t i = sData->texturesCurrentBatchSize;

        float textureWidth = static_cast<float>(texture->getWidth());
        float textureHeight = static_cast<float>(texture->getHeight());

        sData->texturesVertexBufferData[i * 32 + 0] = dest.x;
        sData->texturesVertexBufferData[i * 32 + 1] = dest.y;

        sData->texturesVertexBufferData[i * 32 + 2] = source.x / textureWidth;
        sData->texturesVertexBufferData[i * 32 + 3] = source.y / textureHeight;

        sData->texturesVertexBufferData[i * 32 + 4] = tint.r;
        sData->texturesVertexBufferData[i * 32 + 5] = tint.g;
        sData->texturesVertexBufferData[i * 32 + 6] = tint.b;

        sData->texturesVertexBufferData[i * 32 + 7] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i * 32 + 8] = dest.x + dest.w;
        sData->texturesVertexBufferData[i * 32 + 9] = dest.y;

        sData->texturesVertexBufferData[i * 32 + 10] = (source.x + source.w) / textureWidth;
        sData->texturesVertexBufferData[i * 32 + 11] = source.y / textureHeight;

        sData->texturesVertexBufferData[i * 32 + 12] = tint.r;
        sData->texturesVertexBufferData[i * 32 + 13] = tint.g;
        sData->texturesVertexBufferData[i * 32 + 14] = tint.b;

        sData->texturesVertexBufferData[i * 32 + 15] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i * 32 + 16] = dest.x + dest.w;
        sData->texturesVertexBufferData[i * 32 + 17] = dest.y + dest.h;

        sData->texturesVertexBufferData[i * 32 + 18] = (source.x + source.w) / textureWidth;
        sData->texturesVertexBufferData[i * 32 + 19] = (source.y + source.h) / textureHeight;

        sData->texturesVertexBufferData[i * 32 + 20] = tint.r;
        sData->texturesVertexBufferData[i * 32 + 21] = tint.g;
        sData->texturesVertexBufferData[i * 32 + 22] = tint.b;

        sData->texturesVertexBufferData[i * 32 + 23] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i * 32 + 24] = dest.x;
        sData->texturesVertexBufferData[i * 32 + 25] = dest.y + dest.h;

        sData->texturesVertexBufferData[i * 32 + 26] = source.x / textureWidth;
        sData->texturesVertexBufferData[i * 32 + 27] = (source.y + source.h) / textureHeight;

        sData->texturesVertexBufferData[i * 32 + 28] = tint.r;
        sData->texturesVertexBufferData[i * 32 + 29] = tint.g;
        sData->texturesVertexBufferData[i * 32 + 30] = tint.b;

        sData->texturesVertexBufferData[i * 32 + 31] = static_cast<float>(textureID);

        sData->texturesCurrentBatchSize++;

        if (sData->texturesCurrentBatchSize >= sData->texturesMaxBatchSize) {
            Renderer2D::FlushTextures();
        }
    }

    void Renderer2D::DrawTextureFromID(const Rect& dest, int8_t textureID, const glm::vec3& tint) {
        UNUSED(tint);
        uint32_t i = sData->texturesCurrentBatchSize;

        sData->texturesVertexBufferData[i * 32 + 0] = dest.x;
        sData->texturesVertexBufferData[i * 32 + 1] = dest.y;

        sData->texturesVertexBufferData[i * 32 + 2] = 0;
        sData->texturesVertexBufferData[i * 32 + 3] = 0;

        sData->texturesVertexBufferData[i * 32 + 4] = tint.r;
        sData->texturesVertexBufferData[i * 32 + 5] = tint.g;
        sData->texturesVertexBufferData[i * 32 + 6] = tint.b;

        sData->texturesVertexBufferData[i * 32 + 7] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i * 32 + 8] = dest.x + dest.w;
        sData->texturesVertexBufferData[i * 32 + 9] = dest.y;

        sData->texturesVertexBufferData[i * 32 + 10] = 1;
        sData->texturesVertexBufferData[i * 32 + 11] = 0;

        sData->texturesVertexBufferData[i * 32 + 12] = tint.r;
        sData->texturesVertexBufferData[i * 32 + 13] = tint.g;
        sData->texturesVertexBufferData[i * 32 + 14] = tint.b;

        sData->texturesVertexBufferData[i * 32 + 15] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i * 32 + 16] = dest.x + dest.w;
        sData->texturesVertexBufferData[i * 32 + 17] = dest.y + dest.h;

        sData->texturesVertexBufferData[i * 32 + 18] = 1;
        sData->texturesVertexBufferData[i * 32 + 19] = 1;

        sData->texturesVertexBufferData[i * 32 + 20] = tint.r;
        sData->texturesVertexBufferData[i * 32 + 21] = tint.g;
        sData->texturesVertexBufferData[i * 32 + 22] = tint.b;

        sData->texturesVertexBufferData[i * 32 + 23] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i * 32 + 24] = dest.x;
        sData->texturesVertexBufferData[i * 32 + 25] = dest.y + dest.h;

        sData->texturesVertexBufferData[i * 32 + 26] = 0;
        sData->texturesVertexBufferData[i * 32 + 27] = 1;

        sData->texturesVertexBufferData[i * 32 + 28] = tint.r;
        sData->texturesVertexBufferData[i * 32 + 29] = tint.g;
        sData->texturesVertexBufferData[i * 32 + 30] = tint.b;

        sData->texturesVertexBufferData[i * 32 + 31] = static_cast<float>(textureID);

        sData->texturesCurrentBatchSize++;

        if (sData->texturesCurrentBatchSize >= sData->texturesMaxBatchSize) {
            Renderer2D::FlushTextures();
        }
    }
} // namespace Car
