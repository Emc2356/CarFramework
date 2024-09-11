#pragma once

#include "Car/Core/Core.hpp"

#include "Car/Renderer/VertexArray.hpp"

namespace Car {
    class Renderer {
    public:
        static void Init() { sInstance->InitImpl(); }
        static void Shutdown() { sInstance->ShutdownImpl(); }

        static void ClearColor(float r, float g, float b, float a) { sInstance->ClearColorImpl(r, g, b, a); }
        static void ClearColor(float r, float g, float b) { sInstance->ClearColorImpl(r, g, b, 1.0f); }
        static void ClearColor(float mono, float a) { sInstance->ClearColorImpl(mono, mono, mono, a); }
        static void ClearColor(float mono) { sInstance->ClearColorImpl(mono, mono, mono, 1.0f); }
        // 0xRRGGBBAA
        static void ClearColor(uint32_t hex) {
            sInstance->ClearColorImpl(float((hex >> 24) & 0xFF) / 255, float((hex >> 16) & 0xFF) / 255,
                                      float((hex >> 8) & 0xFF) / 255, float(hex & 0xFF) / 255);
        }

        static void DrawCommand(const Ref<VertexArray> va) {
            sInstance->DrawCommandImpl(va, va->getIndexBuffer()->getCount());
        }
        static void DrawCommand(const Ref<VertexArray> va, uint64_t indicesCount) {
            sInstance->DrawCommandImpl(va, indicesCount);
        }

        static void SetViewport(float x, float y, float width, float height, float minDepth = 0.0f,
                                float maxDepth = 1.0f) {
            sInstance->SetViewportImpl(x, y, width, height, minDepth, maxDepth);
        }
        static void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) {
            sInstance->SetScissorImpl(x, y, width, height);
        }
        static void SetPushConstant(Ref<VertexArray> va, bool vert, bool frag, void* data, uint32_t size, uint32_t offset) {
            sInstance->SetPushConstantImpl(va, vert, frag, data, size, offset);
        }

        // implementation detail
        static void BeginRecording() { sInstance->BeginRecordingImpl(); }
        static void EndRecording() { sInstance->EndRecordingImpl(); }

    protected:
        virtual void InitImpl() = 0;
        virtual void ShutdownImpl() = 0;
        virtual void ClearColorImpl(float r, float g, float b, float a) = 0;
        virtual void DrawCommandImpl(const Ref<VertexArray> va, uint64_t indicesCount) = 0;
        virtual void SetViewportImpl(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
        virtual void SetScissorImpl(int32_t x, int32_t y, int32_t width, int32_t height) = 0;
        virtual void SetPushConstantImpl(Ref<VertexArray> va, bool vert, bool frag, void* data, uint32_t size, uint32_t offset) = 0;
        virtual void BeginRecordingImpl() = 0;
        virtual void EndRecordingImpl() = 0;

    private:
        static Car::Renderer* sInstance;
    };
} // namespace Car
