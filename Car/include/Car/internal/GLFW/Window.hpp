#pragma once

#include "Car/Window.hpp"

#include <GLFW/glfw3.h>


namespace Car {
    class GLFWWindow : public Window {
    public:
        GLFWWindow(const Car::Window::Properties& properties);
        virtual ~GLFWWindow() override;

        virtual void onUpdate() override;

        virtual void setVSync(bool enabled) override { glfwSwapInterval(mProperties.vsync = enabled); }

        virtual void* getNativeWindow() const override { return (void*)mWindow; }

        static Ref<Car::Window> Create(const Car::Window::Properties& properties=Car::Window::Properties());
    private:
        virtual void init(const Car::Window::Properties& properties);
        virtual void shutdown();
    private:
        GLFWwindow* mWindow;
    };
}
