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

		void begin();
		void end();
    };
}
