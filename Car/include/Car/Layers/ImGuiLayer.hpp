#pragma once

#include "Car/Layers/Layer.hpp"

#include "Car/Events/Event.hpp"
#include "Car/Events/KeyEvent.hpp"
#include "Car/Events/MouseEvent.hpp"
#include "Car/Events/WindowEvent.hpp"

namespace Car {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer();
        virtual ~ImGuiLayer() override = default;

        virtual void onAttach() override;
        virtual void onDetach() override;

        virtual bool onMouseButtonPressedEvent(MouseButtonPressedEvent&) override;
        virtual bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent&) override;
        virtual bool onMouseMovedEvent(MouseMovedEvent&) override;
        virtual bool onMouseScrolledEvent(MouseScrolledEvent&) override;
        virtual bool onKeyPressedEvent(KeyPressedEvent&) override;
        virtual bool onKeyReleasedEvent(KeyReleasedEvent&) override;
        virtual bool onKeyTypedEvent(KeyTypedEvent&) override;

        void begin();
        void end();

    private:
        bool mWantCaptureMouse;
        bool mWantCaptureKeyboard;
        bool mWantTextInput;
    };
} // namespace Car
