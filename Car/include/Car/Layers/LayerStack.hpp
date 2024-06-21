#pragma once

#include "Car/Core/Core.hpp"

#include "Car/Layers/Layer.hpp"


namespace Car {
    class LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        void pushLayer(Car::Layer*);
        void popLayer(Car::Layer*);
        void pushOverlay(Car::Layer*);
        void popOverlay(Car::Layer*);

        std::vector<Car::Layer*>::iterator begin() { return mLayers.begin(); }
        std::vector<Car::Layer*>::iterator end() { return mLayers.end(); }
    private:
        std::vector<Car::Layer*> mLayers;
        uint32_t mLayerInsertIndex;
    };
}
