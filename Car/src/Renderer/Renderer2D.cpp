#include "Car/Renderer/Renderer.hpp"
#include "Car/Renderer/Renderer2D.hpp"
#include "Car/Application.hpp"
#include "Car/Core/Core.hpp"
#include "Car/Renderer/Buffer.hpp"
#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/Renderer/VertexArray.hpp"
#include "Car/Renderer/VertexBuffer.hpp"

struct Renderer2DVertex {
    glm::vec2 pos;
    glm::vec2 uv;
    glm::vec3 tint;
    uint32_t textureID;
};

struct Renderer2DData {
    Car::Ref<Car::Shader> shader;
    Car::Ref<Car::IndexBuffer> ib;
    Car::Ref<Car::VertexBuffer> vb;
    Car::Ref<Car::VertexArray> va;
    Car::Ref<Car::Texture2D> nullTexture;
    uint32_t whiteTextureID = 8;

    uint32_t maxBatchSize;
    uint32_t currentBatchSize;
    Renderer2DVertex* vertices;

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

        Shader::VertexInputLayout layout = {
            {"iPos", Shader::VertexInputLayout::DataType::Float2},
            {"iSourceUV", Shader::VertexInputLayout::DataType::Float2},
            {"iTint", Shader::VertexInputLayout::DataType::Float3},
            {"iTextureID", Shader::VertexInputLayout::DataType::UInt},
        };

        assert(sizeof(Renderer2DVertex) == layout.getTotalSize());

        Shader::Specification spec{};
        spec.pushConstantLayout.useInVertexShader = true;
        spec.pushConstantLayout.useInFragmentShader = false;
        spec.pushConstantLayout.size = sizeof(glm::mat4);
        spec.vertexInputLayout = layout;
        spec.vertexInputRate = Shader::VertexInputRate::VERTEX;
        spec.polygonMode = Shader::PolygonMode::FILL;
        spec.cullMode = Shader::CullMode::BACK;
        spec.frontFace = Shader::FrontFace::CLOCKWISE;
        spec.primitiveTopology = Shader::PrimitiveTopology::TRIANGLE_LIST;
        spec.primitiveRestartEnable = false;
        spec.vertexShaderEntryName = "main";
        spec.fragmentShaderEntryName = "main";
        // internal use only so no reason to register with the ResourceManager
        sData->shader = Shader::Create("builtin/Renderer2D.vert", "builtin/Renderer2D.frag", &spec);

        uint32_t nullTextureData = 0xFFFFFFFF;
        sData->nullTexture = Car::Texture2D::Create(1, 1, &nullTextureData);

        for (uint32_t i = 0; i < 8; i++) {
            sData->shader->setInput(0, i, true, sData->nullTexture);
        }

        // TODO: change the batch size so the index buffer can use uint16_t
        // TODO: investigate of uint16_t is better
        sData->maxBatchSize = 20000;
        // sData->texturesMaxBatchSize = 10800;
        sData->currentBatchSize = 0;

        sData->vertices = new Renderer2DVertex[sData->maxBatchSize * 4];
        uint32_t* indexBufferData = new uint32_t[sData->maxBatchSize * 6];

        // initialize the index buffer since that will be the same always
        for (size_t i = 0; i < sData->maxBatchSize; i++) {
            indexBufferData[i * 6 + 0] = i * 4 + 0;
            indexBufferData[i * 6 + 1] = i * 4 + 1;
            indexBufferData[i * 6 + 2] = i * 4 + 2;
            indexBufferData[i * 6 + 3] = i * 4 + 2;
            indexBufferData[i * 6 + 4] = i * 4 + 3;
            indexBufferData[i * 6 + 5] = i * 4 + 0;
        }
        // 0b1_00111000_01111111
        sData->ib = IndexBuffer::Create(indexBufferData, sData->maxBatchSize * 6 * sizeof(uint32_t),
                                        Buffer::Usage::StaticDraw, Buffer::Type::UnsignedInt);

        delete[] indexBufferData;

        sData->vb = VertexBuffer::Create(sData->vertices, sData->maxBatchSize * 4 * sizeof(Renderer2DVertex),
                                         Buffer::Usage::DynamicDraw);

        sData->va = VertexArray::Create(sData->vb, sData->ib, sData->shader);
    }

    void Renderer2D::Shutdown() {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        delete[] sData->vertices;
        delete sData;
    }

    void Renderer2D::Begin() {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        if (sData->currentBatchSize > 0) {
            CR_CORE_ERROR("Called Car::Renderer2D::Begin() without closing the last begin");
        }

        sData->textureTextures.clear();
    }

    void Renderer2D::End() {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        Renderer2D::FlushTextures();
    }

    void Renderer2D::FlushTextures() {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        // no work to be done, early return
        if (sData->currentBatchSize == 0) {
            return;
        }

        auto window = Car::Application::Get()->getWindow();

        glm::mat4 proj = glm::ortho(0.0f, (float)window->getWidth(), 0.0f, (float)window->getHeight(), 1.0f, -1.0f);

        Renderer::SetPushConstant(sData->va, true, false, glm::value_ptr(proj), sizeof(glm::mat4), 0);

        for (size_t i = 0; i < sData->textureTextures.size(); i++) {
            // sData->textureTextures[i]->bind(i);
            sData->shader->setInput(0, i, false, sData->textureTextures[i]);
        }

        sData->vb->updateData((void*)sData->vertices, sData->currentBatchSize * 4 * sizeof(Renderer2DVertex), 0);

        Renderer::DrawCommand(sData->va, sData->currentBatchSize * 2 * 3);
        sData->currentBatchSize = 0;
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
            sData->currentBatchSize = 0;
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

        uint32_t i = sData->currentBatchSize * 4;

        float slope = (float)(end.y - (float)start.y) / ((float)end.x - (float)start.x);
        // TODO: better name
        slope = glm::atan(glm::degrees(slope));

        // Questionable code but works ish for now
        if (start.y > end.y) { // drawing upwards

            sData->vertices[i].pos = {glm::floor(start.x - glm::cos(slope) * lineWidth / 2),
                                      glm::floor(start.y + glm::sin(slope) * lineWidth / 2)};

            sData->vertices[i].tint = color;
            sData->vertices[i].textureID = textureID;
            i++;

            sData->vertices[i].pos = {glm::floor(end.x - glm::cos(slope) * lineWidth / 2),
                                      glm::floor(end.y + glm::sin(slope) * lineWidth / 2)};

            sData->vertices[i].tint = color;
            sData->vertices[i].textureID = textureID;
            i++;

            sData->vertices[i].pos = {glm::floor(end.x + glm::cos(slope) * lineWidth / 2),
                                      glm::floor(end.y - glm::sin(slope) * lineWidth / 2)};

            sData->vertices[i].tint = color;
            sData->vertices[i].textureID = textureID;
            i++;

            sData->vertices[i].pos = {glm::floor(start.x + glm::cos(slope) * lineWidth / 2),
                                      glm::floor(start.y - glm::sin(slope) * lineWidth / 2)};

            sData->vertices[i].tint = color;
            sData->vertices[i].textureID = textureID;
            i++;
        } else { // drawing downwards
            sData->vertices[i].pos = {glm::ceil(start.x + glm::cos(slope) * lineWidth / 2),
                                      glm::floor(start.y - glm::sin(slope) * lineWidth / 2)};
            sData->vertices[i].tint = color;
            sData->vertices[i].textureID = textureID;
            i++;

            sData->vertices[i].pos = {glm::ceil(end.x + glm::cos(slope) * lineWidth / 2),
                                      glm::floor(end.y - glm::sin(slope) * lineWidth / 2)};
            sData->vertices[i].tint = color;
            sData->vertices[i].textureID = textureID;
            i++;

            sData->vertices[i].pos = {glm::ceil(end.x - glm::cos(slope) * lineWidth / 2),
                                      glm::floor(end.y + glm::sin(slope) * lineWidth / 2)};
            sData->vertices[i].tint = color;
            sData->vertices[i].textureID = textureID;
            i++;

            sData->vertices[i].pos = {glm::ceil(start.x - glm::cos(slope) * lineWidth / 2),
                                      glm::floor(start.y + glm::sin(slope) * lineWidth / 2)};
            sData->vertices[i].tint = color;
            sData->vertices[i].textureID = textureID;
            i++;
        }

        sData->currentBatchSize++;

        if (sData->currentBatchSize >= sData->maxBatchSize) {
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

        uint32_t i = sData->currentBatchSize * 4;

        sData->vertices[i].pos = {dest.x, dest.y};
        sData->vertices[i].uv = {source.x / textureWidth, source.y / textureHeight};
        sData->vertices[i].tint = tint;
        sData->vertices[i].textureID = textureID;
        i++;

        sData->vertices[i].pos = {dest.x + dest.w, dest.y};
        sData->vertices[i].uv = {(source.x + source.w) / textureWidth, source.y / textureHeight};
        sData->vertices[i].tint = tint;
        sData->vertices[i].textureID = textureID;
        i++;

        sData->vertices[i].pos = {dest.x + dest.w, dest.y + dest.h};
        sData->vertices[i].uv = {(source.x + source.w) / textureWidth, (source.y + source.h) / textureHeight};
        sData->vertices[i].tint = tint;
        sData->vertices[i].textureID = textureID;
        i++;

        sData->vertices[i].pos = {dest.x, dest.y + dest.h};
        sData->vertices[i].uv = {source.x / textureWidth, (source.y + source.h) / textureHeight};
        sData->vertices[i].tint = tint;
        sData->vertices[i].textureID = textureID;
        i++;

        sData->currentBatchSize++;

        if (sData->currentBatchSize >= sData->maxBatchSize) {
            Renderer2D::FlushTextures();
        }
    }

    void Renderer2D::DrawTextureFromID(const Rect& dest, int8_t textureID, const glm::vec3& tint) {
        _CR_R2_REQ_INIT_OR_RET_VOID();

        uint32_t i = sData->currentBatchSize * 4;

        sData->vertices[i].pos = {dest.x, dest.y};
        sData->vertices[i].uv = {0, 0};
        sData->vertices[i].tint = tint;
        sData->vertices[i].textureID = textureID;
        i++;

        sData->vertices[i].pos = {dest.x + dest.w, dest.y};
        sData->vertices[i].uv = {1, 0};
        sData->vertices[i].tint = tint;
        sData->vertices[i].textureID = textureID;
        i++;

        sData->vertices[i].pos = {dest.x + dest.w, dest.y + dest.h};
        sData->vertices[i].uv = {1, 1};
        sData->vertices[i].tint = tint;
        sData->vertices[i].textureID = textureID;
        i++;

        sData->vertices[i].pos = {dest.x, dest.y + dest.h};
        sData->vertices[i].uv = {0, 1};
        sData->vertices[i].tint = tint;
        sData->vertices[i].textureID = textureID;
        i++;

        sData->currentBatchSize++;

        if (sData->currentBatchSize >= sData->maxBatchSize) {
            Renderer2D::FlushTextures();
        }
    }
} // namespace Car
