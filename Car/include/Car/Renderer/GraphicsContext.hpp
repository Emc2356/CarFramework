#pragma once

#include "Car/Core/Core.hpp"

struct GLFWwindow;

namespace Car {
    class GraphicsContext {
    public:
        virtual ~GraphicsContext() = default;

        virtual void init() = 0;
        virtual void swapBuffers() = 0;
        virtual void resize(uint32_t width, uint32_t height) = 0;

        static Ref<GraphicsContext> Create(GLFWwindow* windowHandle);

        static Ref<GraphicsContext> Get();
    };
} // namespace Car
