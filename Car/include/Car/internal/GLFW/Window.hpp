#pragma once

#include "Car/Window.hpp"

#include <GLFW/glfw3.h>


namespace Car {
    class GLFWWindow : public Window {
    public:
        GLFWWindow(const Car::Window::Properties& properties);
        virtual ~GLFWWindow() override;

        virtual void onUpdate() override;
        
        
        virtual uint32_t getWidth() const override { return mProperties.width; }
        virtual uint32_t getHeight() const override { return mProperties.height; }
        virtual float getAspectRation() const override {return (float)mProperties.width / (float)mProperties.height; }
        virtual const std::string& getTitle() const override { return mProperties.title; }
        virtual bool isVSync() const override { return mProperties.vsync; }
        virtual void setVSync(bool enabled) override { glfwSwapInterval(mProperties.vsync = enabled); }

        virtual void setEventCallback(const eventCallbackFn& callback) override { mProperties.eventCallback = callback; }
        
        virtual void* getNativeWindow() const override { return (void*)mWindow; }

        static Ref<Car::Window> Create(const Car::Window::Properties& properties=Car::Window::Properties());
    private:
        virtual void init(const Car::Window::Properties& properties);
        virtual void shutdown();
    private:
        Car::Window::Properties mProperties;
        Ref<GraphicsContext> mContext;
        GLFWwindow* mWindow;
    };
}
