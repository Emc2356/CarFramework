#pragma once

#include "Car/Events/Event.hpp"

namespace Car {
    class KeyEvent : public Event {
    public:
        inline uint32_t getKeyCode() const { return mKeyCode; }

        CR_EVENT_CLASS_CATEGORY(Car::Event::Category::Keyboard | Car::Event::Category::Input);

    protected:
        KeyEvent(int keycode) : mKeyCode(keycode) {}

        uint32_t mKeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), mRepeatCount(repeatCount) {}

        inline uint32_t getRepeatCount() const { return mRepeatCount; }

        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << mKeyCode << " (" << mRepeatCount << " repeats)";
            return ss.str();
        }

        CR_EVENT_CLASS_TYPE(KeyPressed)
    private:
        uint32_t mRepeatCount;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(uint32_t keycode) : KeyEvent(keycode) {}

        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << mKeyCode;
            return ss.str();
        }

        CR_EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent {
    public:
        KeyTypedEvent(uint32_t keycode) : KeyEvent(keycode) {}

        std::string toString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << mKeyCode;
            return ss.str();
        }

        CR_EVENT_CLASS_TYPE(KeyTyped)
    };
} // namespace Car
