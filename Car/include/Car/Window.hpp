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
            Properties() : width(1280), height(720), title("Car Framework"), vsync(true), eventCallback(nullptr) {}

            uint32_t width;
            uint32_t height;
            std::string title;
            bool vsync;
            eventCallbackFn eventCallback;
        };
    public:
        virtual ~Window() = default;

        virtual void onUpdate() = 0;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual float getAspectRation() const = 0;
        virtual const std::string& getTitle() const = 0;
        virtual bool isVSync() const = 0;

        virtual void setEventCallback(const eventCallbackFn& callback) = 0;
        virtual void setVSync(bool enabled) = 0;

        static Ref<Window> Create(const Properties& properties=Properties());
        virtual void* getNativeWindow() const = 0;
    };
}