#pragma once

#include "Car/Renderer/IndexBuffer.hpp"
#include "Car/Renderer/Renderer.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/VertexArray.hpp"

namespace Car {
    class VulkanRenderer : public Renderer {
    protected:
        virtual void InitImpl() override;
        virtual void ShutdownImpl() override;
        virtual void ClearColorImpl(float r, float g, float b, float a) override;
        virtual void DrawCommandImpl(const Ref<VertexArray> va, uint32_t indicesCount) override;
        virtual void SetViewportImpl(float x, float y, float width, float height, float minDepth,
                                     float maxDepth) override;
        virtual void SetScissorImpl(int32_t x, int32_t y, int32_t width, int32_t height) override;

        virtual void BeginRecordingImpl() override;
        virtual void EndRecordingImpl() override;
    };
} // namespace Car
