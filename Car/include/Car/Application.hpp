#pragma once

#include "Car/Core/Core.hpp"

#include "Car/Core/Timestep.hpp"
#include "Car/Events/Event.hpp"

#include "Car/Layers/Layer.hpp"
#include "Car/Layers/LayerStack.hpp"
#include "Car/Layers/ImGuiLayer.hpp"
#include "Car/Window.hpp"

namespace Car {
    class Application {
    public:
        struct Specification {
            uint32_t width = 1280;
            uint32_t height = 720;
            std::string title = "Vroom";
            int32_t targetFPS = 60; // -1 is unlimited
            bool resizable = true;
            bool useImGui = true;
        };

    public:
        Application();
        virtual ~Application() = 0;

        static void SetSpecification(const Specification& spec);

        void pushLayer(Layer*);
        void popLayer(Layer*);
        void pushOverlay(Layer*);
        void popOverlay(Layer*);

        virtual bool onMouseButtonPressedEvent(MouseButtonPressedEvent& event);
        virtual bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);
        virtual bool onMouseMovedEvent(MouseMovedEvent& event);
        virtual bool onMouseScrolledEvent(MouseScrolledEvent& event);
        virtual bool onKeyPressedEvent(KeyPressedEvent& event);
        virtual bool onKeyReleasedEvent(KeyReleasedEvent& event);
        virtual bool onKeyTypedEvent(KeyTypedEvent& event);
        virtual bool onWindowResizeEvent(WindowResizeEvent& event);
        virtual bool onWindowCloseEvent(WindowCloseEvent& event);
        virtual bool onWindowFocusEvent(WindowFocusEvent& event);
        virtual bool onWindowLostFocusEvent(WindowLostFocusEvent& event);

        virtual void onImGuiRender(double deltaTime) { UNUSED(deltaTime); };

        virtual void onUpdate(double deltaTime) { UNUSED(deltaTime); }
        virtual void onRender() {}

        inline const Ref<Car::Window> getWindow() const { return mWindow; }
        static const Car::Application* Get();

        // meant to be called by EntryPoint
        void run();

    public:
        bool isRunning = true;

    private:
        void onEvent(Car::Event& event);

    private:
        Ref<Car::Window> mWindow;
        Car::ImGuiLayer mImGuiLayer;
        Car::LayerStack mLayerStack;
    };

    // To be defined in CLIENT
    Application* createApplication();
} // namespace Car
