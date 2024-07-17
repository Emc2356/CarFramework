#pragma once

#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/Renderer.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/VertexArray.hpp"


namespace Car {
    class OpenGLRenderer : public Renderer {
    protected:
        virtual void InitImpl() override;
        virtual void ShutdownImpl() override;
        virtual void ClearColorImpl(float r, float g, float b, float a) override;
        virtual void ClearImpl() override;
        virtual void EnableBlendingImpl() override;
        virtual void DisableBlendingImpl() override;
        virtual void EnableDepthTestImpl() override;
        virtual void DisableDepthTestImpl() override;
        virtual void DrawTrianglesImpl(const Ref<VertexArray> va) override;
        virtual void DrawTrianglesImpl(const Ref<VertexArray> va, uint32_t triangleCount) override;
        
        // OpenGL doesnt have that concept
        virtual void BeginRecordingImpl() override {}
        virtual void EndRecordingImpl() override {}
    };
}
