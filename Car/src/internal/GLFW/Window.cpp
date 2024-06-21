#include "Car/internal/GLFW/Window.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include "Car/Core/Log.hpp"
#include "Car/Events/Event.hpp"
#include "Car/Events/KeyEvent.hpp"
#include "Car/Events/MouseEvent.hpp"
#include "Car/Events/WindowEvent.hpp"

// TODO: do this at compile time so it can chose the correct context :/
#if defined(CR_OPENGL)
#include "Car/internal/OpenGL/GraphicsContext.hpp"
#else
#error not implemented yet
#endif /*defined(CR_OPENGL)*/

#include <imgui.h>


namespace Car {
    static bool sIsGLFWInitialized = false;

    void GLFWWindow::onUpdate() {
        mContext->swapBuffers();
        glfwPollEvents();
    }

    GLFWWindow::GLFWWindow(const Car::Window::Properties& properties) {
        mProperties = properties;

        init(mProperties);
        CR_CORE_TRACE("Window created");
    }

    GLFWWindow::~GLFWWindow() {
        shutdown();
    }

    void GLFWWindow::shutdown() {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
    }


    void GLFWWindow::init(const Car::Window::Properties& properties) {
        if (!sIsGLFWInitialized) {
            CR_VERIFY(glfwInit(), "Failed to initialize GLFW");

            #if defined(CR_DEBUG)
                glfwSetErrorCallback([](int error, const char* description) {
                    CR_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
                });
            #endif

            #if defined(CR_OPENGL)
                // opengl 4.6 was released in 2017, at this point **most**
                // graphics drivers support opengl 4.6 
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            #else
            #error not implemented yet
            #endif /*defined(CR_OPENGL)*/

            sIsGLFWInitialized = true;
        }

        mWindow = glfwCreateWindow(properties.width, properties.height, properties.title.c_str(), nullptr, nullptr);

        if (!mWindow) {
            glfwTerminate();
            throw std::runtime_error("Car: Failed to create GLFW window");
        }

        #if defined(CR_OPENGL)
            mContext = new Car::OpenGLGraphicsContext(mWindow);
        #else
        #error not implemented yet
        #endif /*defined(CR_OPENGL)*/
        mContext->init();

        setVSync(properties.vsync);

        // Event Handling
        glfwSetWindowUserPointer(mWindow, &mProperties);
        
        glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
            Window::Properties& properties = *static_cast<Window::Properties*>(glfwGetWindowUserPointer(window));
            properties.width = width;
            properties.height = height;

            WindowResizeEvent event(width, height);

            properties.eventCallback(event);
        });

        glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window) {
            Window::Properties& properties = *static_cast<Window::Properties*>(glfwGetWindowUserPointer(window));
            WindowCloseEvent event;
            properties.eventCallback(event);
        });

        glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            UNUSED(scancode);
            UNUSED(mods);
            Window::Properties& properties = *static_cast<Window::Properties*>(glfwGetWindowUserPointer(window));

            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    properties.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    properties.eventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, 1);
                    properties.eventCallback(event);
                    break;
                }
            }        
        });

        glfwSetCharCallback(mWindow, [](GLFWwindow* window, uint32_t keycode) {
            Window::Properties& properties = *static_cast<Window::Properties*>(glfwGetWindowUserPointer(window));

            KeyTypedEvent event(keycode);
            properties.eventCallback(event);
        });

        glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
            UNUSED(mods);
            Window::Properties& properties = *static_cast<Window::Properties*>(glfwGetWindowUserPointer(window));

            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    properties.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    properties.eventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
            Window::Properties& properties = *static_cast<Window::Properties*>(glfwGetWindowUserPointer(window));

            MouseScrolledEvent event(xOffset, yOffset);
            properties.eventCallback(event);
        });

        glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos) {
            Window::Properties& properties = *static_cast<Window::Properties*>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event(xPos, yPos);
            properties.eventCallback(event);
        });
    }

    Ref<Car::Window> Window::Create(const Window::Properties& properties) {
        return createRef<GLFWWindow>(properties);
    }
}