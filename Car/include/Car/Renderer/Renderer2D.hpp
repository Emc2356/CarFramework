#pragma once

#include "Car/Geometry/Rect.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Core/Core.hpp"


namespace Car::Renderer2D {
    void Init();
    void Shutdown();

    void Begin();
    void End();

    void DrawSubTexture(const Ref<Texture2D>& texture, const Rect& source, const Rect& dest);
    void DrawTexture(const Ref<Texture2D>& texture, const Rect& dest);
    // automatically called by End and by DrawTexture as needed
    void FlushTextures();
}
