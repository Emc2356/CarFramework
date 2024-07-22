#include "Car/Application.hpp"
#include "Car/ResourceManager.hpp"
#include "Car/Renderer/Renderer2D.hpp"
#include "Car/Renderer/Renderer.hpp"
#include "Car/Time.hpp"


namespace Car {
    Application* sInstance = nullptr;
    static Application::Specification sSpec;
    
    void Application::SetSpecification(const Application::Specification& spec) {
        sSpec = spec;
    }

    Application::Application() {
        CR_ASSERT(sInstance == nullptr, "Application already exists");
        CR_CORE_DEBUG("Application created");
        sInstance = this;

        Window::Specification windowSpec = {
            sSpec.width,
            sSpec.height,
            sSpec.title,
            sSpec.vsync,
            sSpec.resizable,
            CR_BIND_FN1(Car::Application::onEvent)
        };
        
        mWindow = createRef<Car::Window>(windowSpec);

        Renderer::Init();
        Renderer2D::Init();
        ResourceManager::Init();
    }

    Application::~Application() {
        for (auto it = mLayerStack.end(); it!= mLayerStack.begin(); ) {
            (*--it)->onDetach();
            CR_CORE_DEBUG("Layer destroyed");
        }
        ResourceManager::Shutdown();
        Renderer2D::Shutdown();
        Renderer::Shutdown();
        CR_CORE_DEBUG("Application shutdown");
    }

    const Application* Application::Get() { return sInstance; }

    void Application::run() {
        if (sSpec.useImGui) {
            mImGuiLayer.onAttach();
        }
        while (isRunning) {
            double time = Time::Get();
            Timestep dt = time - mLastFrameTime;
            mLastFrameTime = time;

            onUpdate((double)dt);

            for (Layer* layer : mLayerStack) {
                layer->onUpdate((double)dt);
            }

            Renderer::BeginRecording();
            Car::Renderer2D::Begin();
            onRender();
            for (Layer* layer : mLayerStack) {
                layer->onRender();
            }
            Car::Renderer2D::End();
            Renderer::EndRecording();

            if (sSpec.useImGui) {
                mImGuiLayer.begin();
                for (Layer* layer : mLayerStack) {
                    layer->onImGuiRender((double)dt);
                }
                onImGuiRender((double)dt);
                mImGuiLayer.end();
            }
            
            mWindow->onUpdate();
        }
        if (sSpec.useImGui) {
            mImGuiLayer.onDetach();
        }
    }

    void Application::onEvent(Event& event) {
        EventDispatcher dispatcher(event);

        dispatcher.dispatch<WindowCloseEvent>(CR_BIND_FN1(Car::Application::onWindowCloseEvent));
        dispatcher.dispatch<MouseButtonPressedEvent>(CR_BIND_FN1(Car::Application::onMouseButtonPressedEvent));
        dispatcher.dispatch<MouseButtonReleasedEvent>(CR_BIND_FN1(Car::Application::onMouseButtonReleasedEvent));
        dispatcher.dispatch<MouseMovedEvent>(CR_BIND_FN1(Car::Application::onMouseMovedEvent));
        dispatcher.dispatch<MouseScrolledEvent>(CR_BIND_FN1(Car::Application::onMouseScrolledEvent));
        dispatcher.dispatch<KeyPressedEvent>(CR_BIND_FN1(Car::Application::onKeyPressedEvent));
        dispatcher.dispatch<KeyReleasedEvent>(CR_BIND_FN1(Car::Application::onKeyReleasedEvent));
        dispatcher.dispatch<KeyTypedEvent>(CR_BIND_FN1(Car::Application::onKeyTypedEvent));
        dispatcher.dispatch<WindowResizeEvent>(CR_BIND_FN1(Car::Application::onWindowResizeEvent));
        dispatcher.dispatch<WindowFocusEvent>(CR_BIND_FN1(Car::Application::onWindowFocusEvent));
        dispatcher.dispatch<WindowLostFocusEvent>(CR_BIND_FN1(Car::Application::onWindowLostFocusEvent));
        
        if (event.isHandled()) {
            return;
        }
        
        if (mImGuiLayer.onEvent(event)) {
            return;
        }
        
        for (auto it = mLayerStack.end(); it!= mLayerStack.begin(); ) {
            (*--it)->onEvent(event);
            if (event.isHandled())
                break;
        }
    }

    void Application::pushLayer(Layer* layer) {
        mLayerStack.pushLayer(layer);
        layer->onAttach();
        CR_CORE_DEBUG("Layer pushed");
    }

    void Application::popLayer(Layer* layer) {
        mLayerStack.popLayer(layer);
        layer->onDetach();
        CR_CORE_DEBUG("Layer poped");
    }

    void Application::pushOverlay(Layer* layer) {
        mLayerStack.pushOverlay(layer);
        layer->onAttach();
        CR_CORE_DEBUG("overlay pushed");
    }

    void Application::popOverlay(Layer* layer) {
        mLayerStack.popOverlay(layer);
        layer->onDetach();
        CR_CORE_DEBUG("overlay poped");
    }

    bool Application::onMouseButtonPressedEvent(MouseButtonPressedEvent&) { return false; }
    bool Application::onMouseButtonReleasedEvent(MouseButtonReleasedEvent&) { return false; }
    bool Application::onMouseMovedEvent(MouseMovedEvent&) { return false; }
    bool Application::onMouseScrolledEvent(MouseScrolledEvent&) { return false; }
    bool Application::onKeyPressedEvent(KeyPressedEvent&) { return false; }
    bool Application::onKeyReleasedEvent(KeyReleasedEvent&) { return false; }
    bool Application::onKeyTypedEvent(KeyTypedEvent&) { return false; }
    bool Application::onWindowResizeEvent(WindowResizeEvent&) { return false; }
    bool Application::onWindowCloseEvent(WindowCloseEvent&) { isRunning = false; return true; }
    bool Application::onWindowFocusEvent(WindowFocusEvent&) { return false; }
    bool Application::onWindowLostFocusEvent(WindowLostFocusEvent&) { return false; }
}
