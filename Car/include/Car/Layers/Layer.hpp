#pragma once

#include "Car/Core/Core.hpp"

#include "Car/Events/Event.hpp"


namespace Car {
    class Layer {
    public:
        Layer(const std::string& debugName="Layer");
        virtual ~Layer();

        virtual void onAttach() {};
        virtual void onDetach() {};
        virtual void onUpdate(double deltaTime) { UNUSED(deltaTime); };
        virtual bool onEvent(Car::Event& event) { UNUSED(event); return false; };
        virtual void onImGuiRender() {};

        inline const std::string& getDebugName() const { return mDebugName; };
    protected:
        std::string mDebugName;
    };
}
