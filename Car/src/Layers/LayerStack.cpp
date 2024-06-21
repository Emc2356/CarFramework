#include "Car/Layers/LayerStack.hpp"

#include <algorithm>


namespace Car {
    LayerStack::LayerStack() : mLayerInsertIndex(0) {}

    LayerStack::~LayerStack() {
        for (Layer* layer : mLayers) {
            delete layer;
        }
    }

    void LayerStack::pushLayer(Layer* layer) {
        mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
		mLayerInsertIndex++;
    }

    void LayerStack::pushOverlay(Layer* overlay) {
        mLayers.emplace_back(overlay);
    }

    void LayerStack::popLayer(Layer* layer) {
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);
		if (it != mLayers.end()) {
			mLayers.erase(it);
			mLayerInsertIndex--;
		}
    }

    void LayerStack::popOverlay(Layer* overlay) {
        auto it = std::find(mLayers.begin(), mLayers.end(), overlay);
        if (it != mLayers.end()) {
            mLayers.erase(it);
        }
    }
}