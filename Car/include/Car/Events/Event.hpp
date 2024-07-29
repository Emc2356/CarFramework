#pragma once

#include "Car/Core/Core.hpp"

#define CR_EVENT_CLASS_TYPE(eventType)                                                                                 \
    static Car::Event::Type getStaticType() { return Type::eventType; }                                                \
    virtual Car::Event::Type getEventType() const override { return getStaticType(); }                                 \
    virtual const char* getName() const override { return #eventType; }

#define CR_EVENT_CLASS_CATEGORY(category)                                                                              \
    virtual uint32_t getCategoryFlags() const override { return category; }

namespace Car {
    class Event {
    public:
        enum class Type {
            None = 0,
            WindowClose,
            WindowResize,
            WindowFocus,
            WindowLostFocus,
            KeyPressed,
            KeyReleased,
            KeyTyped,
            MouseButtonPressed,
            MouseButtonReleased,
            MouseMoved,
            MouseScrolled,
            Count
        };
        enum Category {
            None = 0,
            Window = BIT(0),
            Keyboard = BIT(1),
            Mouse = BIT(2),
            Input = BIT(3),
        };

    public:
        virtual Car::Event::Type getEventType() const = 0;
        virtual uint32_t getCategoryFlags() const = 0;
        virtual const char* getName() const = 0;
        virtual std::string toString() const { return getName(); }

        inline bool isInCategory(Car::Event::Category category) const { return mCategory & category; }
        inline bool isType(Car::Event::Type type) const { return mType == type; }
        inline bool isHandled() const { return mHandled; }

    protected:
        Car::Event::Type mType;
        Car::Event::Category mCategory;
        bool mHandled;

        friend class EventDispatcher;
    };

    class EventDispatcher {
        template <typename T> using EventFn = std::function<bool(T&)>;

    public:
        EventDispatcher(Event& event) : mEvent(event) {}

        template <typename T> bool dispatch(EventFn<T> func) {
            if (mEvent.getEventType() == T::getStaticType()) {
                mEvent.mHandled = func(*(T*)&mEvent);
                return true;
            }
            return false;
        }

    private:
        Event& mEvent;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e) { return os << e.toString(); }
} // namespace Car
