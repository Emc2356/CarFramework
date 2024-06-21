#pragma once

#include "Car/Renderer/GraphicsContext.hpp"


struct GLFWwindow;


namespace Car {
    class OpenGLGraphicsContext : public GraphicsContext {
	public:
		OpenGLGraphicsContext(GLFWwindow* windowHandle);

		virtual void init() override;
		virtual void swapBuffers() override;
	private:
		GLFWwindow* mWindowHandle;
	};
}
