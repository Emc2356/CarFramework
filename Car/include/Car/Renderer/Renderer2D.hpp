#pragma once

#include "Car/Geometry/Rect.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/Font.hpp"
#include "Car/Core/Core.hpp"


namespace Car {
    // it is a class and not a namespace as objects might need to friend this
    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();
    
        static void Begin();
        static void End();
    
        static void DrawTexture(const Ref<Texture2D>& texture, const Rect& dest, const glm::vec3& tint=glm::vec3(1.0f));
        static void DrawTexture(const Ref<Texture2D>& texture, const glm::ivec2& pos, const glm::vec3& tint=glm::vec3(1.0f));
        static void DrawSubTexture(const Ref<Texture2D>& texture, const Rect& source, const Rect& dest, const glm::vec3& tint=glm::vec3(1.0f));
        static void DrawSubTexture(const Ref<Texture2D>& texture, const Rect& source, const glm::ivec2& pos, const glm::vec3& tint=glm::vec3(1.0f));
        static void DrawText(const Ref<Font>& font, const std::string& text, const glm::ivec2& pos, const glm::vec3& color=glm::vec3(1.0f));
        // automatically called by End and by DrawTexture as needed
        static void FlushTextures();
        
        // internal functions that are exposed if you are dealing with a single sprite sheet or only a couple
        // these functions dont validate the textureID that they are getting
        // they flush the textures as needed
        // getTextureID returns -1 if there are already too many textures and it invalidates the current batch
        static int8_t getTextureID(const Ref<Texture2D>& texture);
        static void DrawSubTextureFromID(const Ref<Texture2D>& texture, const Rect& source, const Rect& dest, int8_t textureID, const glm::vec3& tint=glm::vec3(1.0f));
        static void DrawTextureFromID(const Rect& dest, int8_t textureID, const glm::vec3& tint=glm::vec3(1.0f));
    };
}
