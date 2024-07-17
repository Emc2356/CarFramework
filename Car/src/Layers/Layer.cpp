#include "Car/Layers/Layer.hpp"


namespace Car {
    Layer::Layer(const std::string &name) : mDebugName(name) {}

    Layer::~Layer() {}

    bool Layer::onEvent(Event& event) {
        EventDispatcher dispatcher(event);

        dispatcher.dispatch<WindowCloseEvent>(CR_BIND_FN1(Car::Layer::onWindowCloseEvent));
        dispatcher.dispatch<MouseButtonPressedEvent>(CR_BIND_FN1(Car::Layer::onMouseButtonPressedEvent));
        dispatcher.dispatch<MouseButtonReleasedEvent>(CR_BIND_FN1(Car::Layer::onMouseButtonReleasedEvent));
        dispatcher.dispatch<MouseMovedEvent>(CR_BIND_FN1(Car::Layer::onMouseMovedEvent));
        dispatcher.dispatch<MouseScrolledEvent>(CR_BIND_FN1(Car::Layer::onMouseScrolledEvent));
        dispatcher.dispatch<KeyPressedEvent>(CR_BIND_FN1(Car::Layer::onKeyPressedEvent));
        dispatcher.dispatch<KeyReleasedEvent>(CR_BIND_FN1(Car::Layer::onKeyReleasedEvent));
        dispatcher.dispatch<WindowResizeEvent>(CR_BIND_FN1(Car::Layer::onWindowResizeEvent));
        dispatcher.dispatch<WindowFocusEvent>(CR_BIND_FN1(Car::Layer::onWindowFocusEvent));
        dispatcher.dispatch<WindowLostFocusEvent>(CR_BIND_FN1(Car::Layer::onWindowLostFocusEvent));
        
        return event.isHandled();
    }
}
