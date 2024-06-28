#pragma once

#include "Car/OrthographicCamera.hpp"
#include "Car/Renderer/Renderer2D.hpp"


namespace Car {
    class OpenGLRenderer2D : public Renderer2D {
    protected:
        virtual void InitImpl() override;
        virtual void ClearColorImpl(float r, float g, float b, float a) override;
        virtual void ClearImpl() override;
        virtual void BeginSceneImpl(const Car::OrthographicCamera& camera) override;
        virtual void EndSceneImpl() override;
        virtual void EnableBlendingImpl() override;
        virtual void DrawTrianglesImpl(const Ref<VertexArray> va) override;
        virtual void DrawSceneImpl(Ref<Scene> scene) override; 
    };
}
