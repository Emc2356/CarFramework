#include "Car/Renderer/Renderer.hpp"
#include "Car/Renderer/Renderer2D.hpp"
#include "Car/Application.hpp"
#include "Car/Core/Core.hpp"
#include "Car/Renderer/Buffer.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/Renderer/VertexArray.hpp"
#include "Car/Renderer/VertexBuffer.hpp"

struct Renderer2DData {
    Car::Ref<Car::Shader> texturesShader;
    Car::Ref<Car::UniformBuffer> texturesUBO;
    Car::Ref<Car::IndexBuffer> texturesIB;
    Car::Ref<Car::VertexBuffer> texturesVB;
    Car::Ref<Car::VertexArray> texturesVA;
    uint32_t whiteTextureID = 8;

    uint32_t texturesMaxBatchSize;
    uint32_t texturesCurrentBatchSize;
    float* texturesVertexBufferData;
    uint32_t* texturesIndexBufferData;

    std::vector<Car::Ref<Car::Texture2D>> textureTextures;
};

#define _CR_R2_REQ_INIT_OR_RET(__ret_v)                                                                                \
    CR_IF (sData == nullptr) {                                                                                         \
        CR_CORE_ERROR("Car::Renderer2D is not initialized");                                                           \
        CR_DEBUGBREAK();                                                                                               \
        return __ret_v;                                                                                                \
    }
#define _CR_R2_REQ_INIT_OR_RET_VOID()                                                                                  \
    CR_IF (sData == nullptr) {                                                                                         \
        CR_CORE_ERROR("Car::Renderer2D is not initialized");                                                           \
        CR_DEBUGBREAK();                                                                                               \
        return;                                                                                                        \
    }

namespace Car {
    static Renderer2DData* sData = nullptr;

    void Renderer2D::Init() {
        sData = new Renderer2DData();
        // internal use only so no reason to register with the ResourceManager
        sData->texturesShader = Shader::Create("builtin/Renderer2D.vert", "builtin/Renderer2D.frag");
        sData->texturesUBO = UniformBuffer::Create(sizeof(glm::mat4), 0, Buffer::Usage::DynamicDraw);

        // TODO: change the batch size so the index buffer can use uint16_t
        // TODO: investigate of uint16_t is better
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
        sData->texturesIB = IndexBuffer::Create((void*)sData->texturesIndexBufferData,
                                                sData->texturesMaxBatchSize * 6 * sizeof(uint32_t),
                                                Buffer::Usage::StaticDraw, Buffer::Type::UnsignedInt);

        sData->texturesVB = VertexBuffer::Create(nullptr, sData->texturesMaxBatchSize * 4 * layout.getTotalSize(),
                                                 layout, Buffer::Usage::DynamicDraw);

        sData->texturesVA = VertexArray::Create(sData->texturesVB, sData->texturesIB, sData->texturesShader);
    }

    void Renderer2D::Shutdown() {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        free(sData->texturesVertexBufferData);
        free(sData->texturesIndexBufferData);
        delete sData;
    }

    void Renderer2D::Begin() {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        if (sData->texturesCurrentBatchSize > 0) {
            CR_CORE_ERROR("Called Car::Renderer2D::Begin() without closing the last begin");
        }
    }

    void Renderer2D::End() {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        Renderer2D::FlushTextures();
    }

    void Renderer2D::FlushTextures() {
        _CR_R2_REQ_INIT_OR_RET_VOID();

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
        _CR_R2_REQ_INIT_OR_RET(-1);

        for (size_t i = 0; i < sData->textureTextures.size(); i++) {
            // small trick to save some performace here and there for the vtable
            // lookup
            if ((void*)sData->textureTextures[i].get() == (void*)texture.get()) {
                return i;
            }
        }
        if (sData->textureTextures.size() >= 8) {
            // invalidate the data to make sure this is resolved
            sData->texturesCurrentBatchSize = 0;
            return -1;
        }
        sData->textureTextures.push_back(texture);
        return sData->textureTextures.size() - 1;
    }

    void Renderer2D::DrawTexture(const Ref<Texture2D>& texture, const Rect& dest, const glm::vec3& tint) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawTexture too many textures sent, maximum as "
                          "8, call Car::Renderer2D::FlushTextures if you plan on using "
                          "more than 8 textures");
            return;
        }

        Renderer2D::DrawTextureFromID(dest, textureID, tint);
    }

    void Renderer2D::DrawTexture(const Ref<Texture2D>& texture, const glm::vec2& pos, const glm::vec3& tint) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawTexture too many textures sent, maximum as "
                          "8, call Car::Renderer2D::FlushTextures if you plan on using "
                          "more than 8 textures");
            return;
        }

        Renderer2D::DrawTextureFromID({pos.x, pos.y, (float)texture->getWidth(), (float)texture->getHeight()},
                                      textureID, tint);
    }

    void Renderer2D::DrawSubTexture(const Ref<Texture2D>& texture, const Rect& source, const Rect& dest,
                                    const glm::vec3& tint) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawSubTexture too many textures sent, maximum "
                          "as 8, call Car::Renderer2D::FlushTextures if you plan on using "
                          "more than 8 textures");
            return;
        }

        Renderer2D::DrawSubTextureFromID(texture->getWidth(), texture->getHeight(), source, dest, textureID, tint);
    }

    void Renderer2D::DrawSubTexture(const Ref<Texture2D>& texture, const Rect& source, const glm::vec2& pos,
                                    const glm::vec3& tint) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawSubTexture too many textures sent, maximum "
                          "as 8, call Car::Renderer2D::FlushTextures if you plan on using "
                          "more than 8 textures");
            return;
        }

        Renderer2D::DrawSubTextureFromID(texture->getWidth(), texture->getHeight(), source,
                                         {pos.x, pos.y, source.w - source.x, source.h - source.w}, textureID, tint);
    }

    void Renderer2D::DrawRect(const Rect& rect, const glm::vec3& color) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        Renderer2D::DrawTextureFromID(rect, sData->whiteTextureID, color);
    }

    void Renderer2D::DrawLine(glm::vec2 start_, glm::vec2 end_, const glm::vec3& color, float lineWidth) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        int8_t textureID = sData->whiteTextureID;

        glm::ivec2 start = start_;
        glm::ivec2 end = end_;

        if (start.x == end.x) { // vertical line
            Renderer2D::DrawTextureFromID(
                {start.x - lineWidth / 2, MIN(start_.y, end_.y), lineWidth / 2, ABS(start_.y - end_.y)}, textureID,
                color);
            return;
        } else if (start.y == end.y) { // horizontal line
            Renderer2D::DrawTextureFromID(
                {MIN(start_.x, end_.x), start.y - lineWidth / 2, ABS(start_.x - end_.x), lineWidth / 2}, textureID,
                color);
            return;
        }

        uint32_t i = sData->texturesCurrentBatchSize * 32;

        float slope = (float)(end.y - (float)start.y) / ((float)end.x - (float)start.x);
        // TODO: better name
        slope = glm::atan(glm::degrees(slope));

        sData->texturesVertexBufferData[i + 4] = color.r;
        sData->texturesVertexBufferData[i + 5] = color.g;
        sData->texturesVertexBufferData[i + 6] = color.b;
        sData->texturesVertexBufferData[i + 7] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i + 12] = color.r;
        sData->texturesVertexBufferData[i + 13] = color.g;
        sData->texturesVertexBufferData[i + 14] = color.b;
        sData->texturesVertexBufferData[i + 15] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i + 20] = color.r;
        sData->texturesVertexBufferData[i + 21] = color.g;
        sData->texturesVertexBufferData[i + 22] = color.b;
        sData->texturesVertexBufferData[i + 23] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i + 28] = color.r;
        sData->texturesVertexBufferData[i + 29] = color.g;
        sData->texturesVertexBufferData[i + 30] = color.b;
        sData->texturesVertexBufferData[i + 31] = static_cast<float>(textureID);

        // Questionable code but works ish for now
        if (start.y > end.y) { // drawing upwards
            sData->texturesVertexBufferData[i + 0] = glm::floor(start.x + glm::cos(slope) * lineWidth / 2);
            sData->texturesVertexBufferData[i + 1] = glm::floor(start.y - glm::sin(slope) * lineWidth / 2);

            sData->texturesVertexBufferData[i + 8] = glm::floor(end.x + glm::cos(slope) * lineWidth / 2);
            sData->texturesVertexBufferData[i + 9] = glm::floor(end.y - glm::sin(slope) * lineWidth / 2);

            sData->texturesVertexBufferData[i + 16] = glm::floor(end.x - glm::cos(slope) * lineWidth / 2);
            sData->texturesVertexBufferData[i + 17] = glm::floor(end.y + glm::sin(slope) * lineWidth / 2);

            sData->texturesVertexBufferData[i + 24] = glm::floor(start.x - glm::cos(slope) * lineWidth / 2);
            sData->texturesVertexBufferData[i + 25] = glm::floor(start.y + glm::sin(slope) * lineWidth / 2);
        } else { // drawing downwards
            sData->texturesVertexBufferData[i + 0] = glm::ceil(start.x - glm::cos(slope) * lineWidth / 2);
            sData->texturesVertexBufferData[i + 1] = glm::floor(start.y + glm::sin(slope) * lineWidth / 2);

            sData->texturesVertexBufferData[i + 8] = glm::ceil(end.x - glm::cos(slope) * lineWidth / 2);
            sData->texturesVertexBufferData[i + 9] = glm::floor(end.y + glm::sin(slope) * lineWidth / 2);

            sData->texturesVertexBufferData[i + 16] = glm::ceil(end.x + glm::cos(slope) * lineWidth / 2);
            sData->texturesVertexBufferData[i + 17] = glm::floor(end.y - glm::sin(slope) * lineWidth / 2);

            sData->texturesVertexBufferData[i + 24] = glm::ceil(start.x + glm::cos(slope) * lineWidth / 2);
            sData->texturesVertexBufferData[i + 25] = glm::floor(start.y - glm::sin(slope) * lineWidth / 2);
        }

        sData->texturesCurrentBatchSize++;

        if (sData->texturesCurrentBatchSize >= sData->texturesMaxBatchSize) {
            Renderer2D::FlushTextures();
        }
    }

    void Renderer2D::DrawPoint(const glm::vec2& pos, const glm::ivec3& color) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        // TODO: It draws a rectangle right now :/
        int8_t textureID = sData->whiteTextureID;
        uint32_t i = sData->texturesCurrentBatchSize * 32;

        sData->texturesVertexBufferData[i++] = pos.x - 1;
        sData->texturesVertexBufferData[i++] = pos.y - 1;

        sData->texturesVertexBufferData[i++] = color.r;
        sData->texturesVertexBufferData[i++] = color.g;
        sData->texturesVertexBufferData[i++] = color.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = pos.x + 1;
        sData->texturesVertexBufferData[i++] = pos.y - 1;

        sData->texturesVertexBufferData[i++] = color.r;
        sData->texturesVertexBufferData[i++] = color.g;
        sData->texturesVertexBufferData[i++] = color.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = pos.x + 1;
        sData->texturesVertexBufferData[i++] = pos.y + 1;

        sData->texturesVertexBufferData[i++] = color.r;
        sData->texturesVertexBufferData[i++] = color.g;
        sData->texturesVertexBufferData[i++] = color.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = pos.x - 1;
        sData->texturesVertexBufferData[i++] = pos.y + 1;

        sData->texturesVertexBufferData[i++] = color.r;
        sData->texturesVertexBufferData[i++] = color.g;
        sData->texturesVertexBufferData[i++] = color.b;

        sData->texturesCurrentBatchSize++;

        if (sData->texturesCurrentBatchSize >= sData->texturesMaxBatchSize) {
            Renderer2D::FlushTextures();
        }
    }

    void Renderer2D::DrawText(const Ref<Font>& font, const std::string& text, const glm::vec2& pos,
                              const glm::vec3& color) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        Ref<Texture2D> texture = font->getTexture();

        int8_t textureID = getTextureID(texture);

        if (textureID == -1) {
            CR_CORE_ERROR("Car::Renderer2D::DrawText too many textures sent, maximum as 8, "
                          "call Car::Renderer2D::FlushTextures if you plan on using more "
                          "than 8 textures");
            return;
        }

        float fontHeight = font->mHeight;

        const uint32_t textureWidth = texture->getWidth();
        const uint32_t textureHeight = texture->getHeight();

        float x = pos.x;
        float y = pos.y;

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

            Renderer2D::DrawSubTextureFromID(textureWidth, textureHeight, character.rect,
                                             {x, y, character.rect.w, fontHeight}, textureID, color);

            x += character.advance;
        }
    }

    void Renderer2D::DrawSubTextureFromID(const uint32_t textureWidth, const uint32_t textureHeight, const Rect& source,
                                          const Rect& dest, int8_t textureID, const glm::vec3& tint) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        uint32_t i = sData->texturesCurrentBatchSize * 32;

        sData->texturesVertexBufferData[i++] = dest.x;
        sData->texturesVertexBufferData[i++] = dest.y;

        sData->texturesVertexBufferData[i++] = source.x / textureWidth;
        sData->texturesVertexBufferData[i++] = source.y / textureHeight;

        sData->texturesVertexBufferData[i++] = tint.r;
        sData->texturesVertexBufferData[i++] = tint.g;
        sData->texturesVertexBufferData[i++] = tint.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = dest.x + dest.w;
        sData->texturesVertexBufferData[i++] = dest.y;

        sData->texturesVertexBufferData[i++] = (source.x + source.w) / textureWidth;
        sData->texturesVertexBufferData[i++] = source.y / textureHeight;

        sData->texturesVertexBufferData[i++] = tint.r;
        sData->texturesVertexBufferData[i++] = tint.g;
        sData->texturesVertexBufferData[i++] = tint.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = dest.x + dest.w;
        sData->texturesVertexBufferData[i++] = dest.y + dest.h;

        sData->texturesVertexBufferData[i++] = (source.x + source.w) / textureWidth;
        sData->texturesVertexBufferData[i++] = (source.y + source.h) / textureHeight;

        sData->texturesVertexBufferData[i++] = tint.r;
        sData->texturesVertexBufferData[i++] = tint.g;
        sData->texturesVertexBufferData[i++] = tint.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = dest.x;
        sData->texturesVertexBufferData[i++] = dest.y + dest.h;

        sData->texturesVertexBufferData[i++] = source.x / textureWidth;
        sData->texturesVertexBufferData[i++] = (source.y + source.h) / textureHeight;

        sData->texturesVertexBufferData[i++] = tint.r;
        sData->texturesVertexBufferData[i++] = tint.g;
        sData->texturesVertexBufferData[i++] = tint.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesCurrentBatchSize++;

        if (sData->texturesCurrentBatchSize >= sData->texturesMaxBatchSize) {
            Renderer2D::FlushTextures();
        }
    }

    void Renderer2D::DrawTextureFromID(const Rect& dest, int8_t textureID, const glm::vec3& tint) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        uint32_t i = sData->texturesCurrentBatchSize * 32;

        sData->texturesVertexBufferData[i++] = dest.x;
        sData->texturesVertexBufferData[i++] = dest.y;

        sData->texturesVertexBufferData[i++] = 0;
        sData->texturesVertexBufferData[i++] = 0;

        sData->texturesVertexBufferData[i++] = tint.r;
        sData->texturesVertexBufferData[i++] = tint.g;
        sData->texturesVertexBufferData[i++] = tint.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = dest.x + dest.w;
        sData->texturesVertexBufferData[i++] = dest.y;

        sData->texturesVertexBufferData[i++] = 1;
        sData->texturesVertexBufferData[i++] = 0;

        sData->texturesVertexBufferData[i++] = tint.r;
        sData->texturesVertexBufferData[i++] = tint.g;
        sData->texturesVertexBufferData[i++] = tint.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = dest.x + dest.w;
        sData->texturesVertexBufferData[i++] = dest.y + dest.h;

        sData->texturesVertexBufferData[i++] = 1;
        sData->texturesVertexBufferData[i++] = 1;

        sData->texturesVertexBufferData[i++] = tint.r;
        sData->texturesVertexBufferData[i++] = tint.g;
        sData->texturesVertexBufferData[i++] = tint.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesVertexBufferData[i++] = dest.x;
        sData->texturesVertexBufferData[i++] = dest.y + dest.h;

        sData->texturesVertexBufferData[i++] = 0;
        sData->texturesVertexBufferData[i++] = 1;

        sData->texturesVertexBufferData[i++] = tint.r;
        sData->texturesVertexBufferData[i++] = tint.g;
        sData->texturesVertexBufferData[i++] = tint.b;

        sData->texturesVertexBufferData[i++] = static_cast<float>(textureID);

        sData->texturesCurrentBatchSize++;

        if (sData->texturesCurrentBatchSize >= sData->texturesMaxBatchSize) {
            Renderer2D::FlushTextures();
        }
    }
} // namespace Car
