#include "Car/Window.hpp"

#include "Car/Events/KeyEvent.hpp"
#include "Car/Events/MouseEvent.hpp"
#include "Car/Events/WindowEvent.hpp"

#include <GLFW/glfw3.h>

namespace Car {
    static bool sIsGLFWInitialized = false;

    void Window::onUpdate() {
        mGraphicsContext->swapBuffers();
        glfwPollEvents();
    }

    Window::Window(const Car::Window::Specification& spec) {
        mSpec = spec;

        if (!sIsGLFWInitialized) {
            CR_VERIFY(glfwInit(), "Failed to initialize GLFW");

#if defined(CR_DEBUG)
            glfwSetErrorCallback(
                [](int error, const char* description) { CR_CORE_ERROR("GLFW Error ({0}): {1}", error, description); });
#endif

#if defined(CR_VULKAN)
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#else
#error not implemented yet
#endif

            glfwWindowHint(GLFW_RESIZABLE, spec.resizable ? GLFW_TRUE : GLFW_FALSE);

            sIsGLFWInitialized = true;
        }

        mHandle = glfwCreateWindow(spec.width, spec.height, spec.title.c_str(), nullptr, nullptr);

        if (!mHandle) {
            glfwTerminate();
            throw std::runtime_error("Car: Failed to create GLFW window");
        }

        mGraphicsContext = GraphicsContext::Create(mHandle);
    }

    void Window::init() {
        mGraphicsContext->init();

        // Event Handling
        glfwSetWindowUserPointer(mHandle, &mSpec);

        glfwSetWindowSizeCallback(mHandle, [](GLFWwindow* window, int width, int height) {
            Window::Specification& spec = *static_cast<Window::Specification*>(glfwGetWindowUserPointer(window));
            spec.width = width;
            spec.height = height;

            GraphicsContext::Get()->resize(width, height);

            WindowResizeEvent event(width, height);

            spec.eventCallback(event);
        });

        glfwSetWindowCloseCallback(mHandle, [](GLFWwindow* window) {
            Window::Specification& spec = *static_cast<Window::Specification*>(glfwGetWindowUserPointer(window));
            WindowCloseEvent event;
            spec.eventCallback(event);
        });

        glfwSetKeyCallback(mHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            UNUSED(scancode);
            UNUSED(mods);
            Window::Specification& spec = *static_cast<Window::Specification*>(glfwGetWindowUserPointer(window));

            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, 0);
                spec.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                spec.eventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, 1);
                spec.eventCallback(event);
                break;
            }
            }
        });

        glfwSetCharCallback(mHandle, [](GLFWwindow* window, uint32_t keycode) {
            Window::Specification& spec = *static_cast<Window::Specification*>(glfwGetWindowUserPointer(window));

            KeyTypedEvent event(keycode);
            spec.eventCallback(event);
        });

        glfwSetMouseButtonCallback(mHandle, [](GLFWwindow* window, int button, int action, int mods) {
            UNUSED(mods);
            Window::Specification& spec = *static_cast<Window::Specification*>(glfwGetWindowUserPointer(window));

            switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                spec.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                spec.eventCallback(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(mHandle, [](GLFWwindow* window, double xOffset, double yOffset) {
            Window::Specification& spec = *static_cast<Window::Specification*>(glfwGetWindowUserPointer(window));

            MouseScrolledEvent event(xOffset, yOffset);
            spec.eventCallback(event);
        });

        glfwSetCursorPosCallback(mHandle, [](GLFWwindow* window, double xPos, double yPos) {
            Window::Specification& spec = *static_cast<Window::Specification*>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event(xPos, yPos);
            spec.eventCallback(event);
        });

        CR_CORE_DEBUG("Window initialized");
    }

    Window::~Window() {
        glfwDestroyWindow(mHandle);
        glfwTerminate();
        mHandle = nullptr;
        CR_CORE_DEBUG("Window shutdown");
    }

    std::vector<const char*> Window::getRequiredInstanceExtensions() const {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> ret(glfwExtensionCount);

        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            ret[i] = glfwExtensions[i];
        }

        return ret;
    }
} // namespace Car
