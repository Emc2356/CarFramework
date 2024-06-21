#pragma once

#include "Car/Core/Core.hpp"


namespace Car {
    class GraphicsContext {
	public:
		virtual void init() = 0;
		virtual void swapBuffers() = 0;
    };
}
