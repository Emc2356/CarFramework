#pragma once

#include "Car/OrthographicCamera.hpp"
#include "Car/Core/Core.hpp"

#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/Renderer/VertexArray.hpp"


namespace Car {
    class Renderer {
    public:
        static void Init() { sInstance->InitImpl(); }

        static void ClearColor(float r, float g, float b, float a) { sInstance->ClearColorImpl(r, g, b, a); }
        static void ClearColor(float r, float g, float b) { sInstance->ClearColorImpl(r, g, b, 1.0f); }
        static void ClearColor(float mono, float a) { sInstance->ClearColorImpl(mono, mono, mono, a); }
        static void ClearColor(float mono) { sInstance->ClearColorImpl(mono, mono, mono, 1.0f); }
        // 0xRRGGBBAA
        static void ClearColor(uint32_t hex) { sInstance->ClearColorImpl(float((hex >> 24) & 0xFF) / 255, float((hex >> 16) & 0xFF) / 255, float((hex >> 8) & 0xFF) / 255, float(hex & 0xFF) / 255); }

        static void Clear() { sInstance->ClearImpl(); }

        static void BeginScene(const Car::OrthographicCamera& camera) { sInstance->BeginSceneImpl(camera); }
        static void EndScene() { sInstance->EndSceneImpl(); }

        static void EnableBlending() { sInstance->EnableBlendingImpl(); }

        static void DrawTriangles(const Ref<VertexArray> va) { sInstance->DrawTrianglesImpl(va); }
    protected:
        virtual void InitImpl() = 0;
        virtual void ClearColorImpl(float r, float g, float b, float a) = 0;
        virtual void ClearImpl() = 0;
        virtual void BeginSceneImpl(const Car::OrthographicCamera& camera) = 0;
        virtual void EndSceneImpl() = 0;
        virtual void EnableBlendingImpl() = 0;
        virtual void DrawTrianglesImpl(const Ref<VertexArray> va) = 0;
    protected:
        Ref<UniformBuffer> mCarUniformBuffer;
    private:
        static Car::Renderer* sInstance;
    };
}
