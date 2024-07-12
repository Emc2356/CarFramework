#pragma once

#include "Car/Renderer/GraphicsContext.hpp"


struct GLFWwindow;


namespace Car {
    class OpenGLGraphicsContext : public GraphicsContext {
	public:
		OpenGLGraphicsContext(GLFWwindow* windowHandle);
		virtual ~OpenGLGraphicsContext() override = default;

		virtual void init() override;
		virtual void swapBuffers() override;
		virtual void resize(uint32_t width, uint32_t height) override;
	private:
		GLFWwindow* mWindowHandle;
	};
}
