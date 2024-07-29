#pragma once

#include "Car/Events/Event.hpp"

namespace Car {
    class WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height) : mWidth(width), mHeight(height) {}

        inline uint32_t getWidth() const { return mWidth; }
        inline uint32_t getHeight() const { return mHeight; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
            return ss.str();
        }

        CR_EVENT_CLASS_TYPE(WindowResize)
        CR_EVENT_CLASS_CATEGORY(Car::Event::Category::Window)
    private:
        uint32_t mWidth, mHeight;
    };

    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() {}

        CR_EVENT_CLASS_TYPE(WindowClose)
        CR_EVENT_CLASS_CATEGORY(Car::Event::Category::Window)
    };

    class WindowFocusEvent : public Event {
    public:
        WindowFocusEvent() {}

        CR_EVENT_CLASS_TYPE(WindowFocus)
        CR_EVENT_CLASS_CATEGORY(Car::Event::Category::Window)
    };

    class WindowLostFocusEvent : public Event {
    public:
        WindowLostFocusEvent() {}

        CR_EVENT_CLASS_TYPE(WindowLostFocus)
        CR_EVENT_CLASS_CATEGORY(Car::Event::Category::Window)
    };
} // namespace Car
