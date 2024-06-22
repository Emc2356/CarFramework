#pragma once


#include "Car/Core/Core.hpp"

#include "Car/Events/Event.hpp"
#include "Car/Renderer/GraphicsContext.hpp"


namespace Car {
    class Window {
    public:
        struct Specification {
            uint32_t width;
            uint32_t height;
            std::string title;
            bool vsync;
        };
    public:
        using eventCallbackFn = std::function<bool(Car::Event&)>;

        struct Properties {
            Properties(uint32_t width, uint32_t height, const std::string& title, bool vsync, eventCallbackFn eventCallback)
                : width(width), height(height), title(title), vsync(vsync), eventCallback(eventCallback) {}
            Properties(uint32_t width, uint32_t height, const std::string& title, bool vsync)
                : width(width), height(height), title(title), vsync(vsync), eventCallback(nullptr) {}
            Properties() : width(1280), height(720), title("Car Engine"), vsync(true), eventCallback(nullptr) {}

            uint32_t width;
            uint32_t height;
            std::string title;
            bool vsync;
            eventCallbackFn eventCallback;
        };
    public:
        virtual ~Window() {};

        virtual void onUpdate() = 0;

        uint32_t getWidth() const { return mProperties.width; }
        uint32_t getHeight() const { return mProperties.height; }
        float getAspectRation() const {return (float)mProperties.width / (float)mProperties.height; }
        const std::string& getTitle() const { return mProperties.title; }
        bool isVSync() const { return mProperties.vsync; }

        void setEventCallback(const eventCallbackFn& callback) { mProperties.eventCallback = callback; }
        virtual void setVSync(bool enabled) = 0;

        static Ref<Window> Create(const Properties& properties=Properties());

        virtual void* getNativeWindow() const = 0;
    protected:
        GraphicsContext* mContext;
        Car::Window::Properties mProperties;
    };
}