#pragma once

#include "Car/Core/Core.hpp"

#include "Car/Events/Event.hpp"
#include "Car/Events/KeyEvent.hpp"
#include "Car/Events/MouseEvent.hpp"
#include "Car/Events/WindowEvent.hpp"

namespace Car {
    class Layer {
    public:
        Layer(const std::string& debugName = "Layer");
        virtual ~Layer();

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate(double deltaTime) { UNUSED(deltaTime); }
        virtual void onRender() {}
        virtual void onImGuiRender(double deltaTime) { UNUSED(deltaTime); }

        virtual bool onMouseButtonPressedEvent(MouseButtonPressedEvent&) { return false; }
        virtual bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent&) { return false; }
        virtual bool onMouseMovedEvent(MouseMovedEvent&) { return false; }
        virtual bool onMouseScrolledEvent(MouseScrolledEvent&) { return false; }
        virtual bool onKeyPressedEvent(KeyPressedEvent&) { return false; }
        virtual bool onKeyReleasedEvent(KeyReleasedEvent&) { return false; }
        virtual bool onKeyTypedEvent(KeyTypedEvent&) { return false; }
        virtual bool onWindowResizeEvent(WindowResizeEvent&) { return false; }
        virtual bool onWindowCloseEvent(WindowCloseEvent&) { return false; }
        virtual bool onWindowFocusEvent(WindowFocusEvent&) { return false; }
        virtual bool onWindowLostFocusEvent(WindowLostFocusEvent&) { return false; }

        inline const std::string& getDebugName() const { return mDebugName; };

        // internal function
        bool onEvent(Car::Event& event);

    protected:
        std::string mDebugName;
    };
} // namespace Car
