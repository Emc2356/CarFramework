#pragma once

#include "Car/Geometry/Rect.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Core/Core.hpp"


namespace Car {
    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        static void Begin();
        static void End();

        static void DrawSubTexture(const Ref<Texture2D>& texture, const Rect& source, const Rect& dest);
        static void DrawTexture(const Ref<Texture2D>& texture, const Rect& dest);
        // automatically called by End and by DrawTexture as needed
        static void FlushTextures();
    };
}
