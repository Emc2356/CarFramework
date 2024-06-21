#pragma once

#include "Car/OrthographicCamera.hpp"
#include "Car/Renderer/Renderer.hpp"


namespace Car {
    class OpenGLRenderer : Renderer {
    protected:
        virtual void InitImpl() override;
        virtual void ClearColorImpl(float r, float g, float b, float a) override;
        virtual void ClearImpl() override;
        virtual void BeginSceneImpl(const Car::OrthographicCamera& camera) override;
        virtual void EndSceneImpl() override;
        virtual void EnableBlendingImpl() override;
        virtual void DrawTrianglesImpl(const Ref<VertexArray> va) override;
    };
}
