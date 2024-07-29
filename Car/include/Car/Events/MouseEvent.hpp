#pragma once

#include "Car/Events/Event.hpp"

namespace Car {
    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(double x, double y) : mMouseX(x), mMouseY(y) {}

        inline double getX() const { return mMouseX; }
        inline double getY() const { return mMouseY; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << mMouseX << ", " << mMouseY;
            return ss.str();
        }

        CR_EVENT_CLASS_TYPE(MouseMoved)
        CR_EVENT_CLASS_CATEGORY(Car::Event::Category::Mouse | Car::Event::Category::Input)
    private:
        double mMouseX, mMouseY;
    };

    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(double xOffset, double yOffset) : mXOffset(xOffset), mYOffset(yOffset) {}

        inline double getXOffset() const { return mXOffset; }
        inline double getYOffset() const { return mYOffset; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
            return ss.str();
        }

        CR_EVENT_CLASS_TYPE(MouseScrolled)
        CR_EVENT_CLASS_CATEGORY(Car::Event::Category::Mouse | Car::Event::Category::Input)
    private:
        double mXOffset, mYOffset;
    };

    class MouseButtonEvent : public Event {
    public:
        inline int32_t getMouseButton() const { return mButton; }

        CR_EVENT_CLASS_CATEGORY(Car::Event::Category::Mouse | Car::Event::Category::Input)
    protected:
        MouseButtonEvent(int32_t button) : mButton(button) {}

        int32_t mButton;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << mButton;
            return ss.str();
        }

        CR_EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << mButton;
            return ss.str();
        }

        CR_EVENT_CLASS_TYPE(MouseButtonReleased)
    };

} // namespace Car