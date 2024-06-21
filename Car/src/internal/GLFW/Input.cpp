#include "Car/internal/GLFW/Input.hpp"

#include "Car/Application.hpp"

#include <GLFW/glfw3.h>


namespace Car {
    Input* Input::sInstance = new GLFWInput();

    bool GLFWInput::IsKeyPressedImpl(uint32_t keycode) {
        GLFWwindow* window = (GLFWwindow*)Application::Get()->getWindow()->getNativeWindow();

        int state = glfwGetKey(window, keycode);

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool GLFWInput::IsMouseButtonPressedImpl(int button) {
        GLFWwindow* window = (GLFWwindow*)Application::Get()->getWindow()->getNativeWindow();

        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    void GLFWInput::MouseXImpl(int32_t* mX) {
        GLFWwindow* window = (GLFWwindow*)Application::Get()->getWindow()->getNativeWindow();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        *mX = (int32_t)mXd;

        UNUSED(mYd);
    }

    void GLFWInput::MouseYImpl(int32_t* mY)  {
        GLFWwindow* window = (GLFWwindow*)Application::Get()->getWindow()->getNativeWindow();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        *mY = (int32_t)mYd;

        UNUSED(mXd);
    }

    void GLFWInput::MousePosImpl(int32_t* mX, int32_t* mY)  {
        GLFWwindow* window = (GLFWwindow*)Application::Get()->getWindow()->getNativeWindow();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        *mX = (int32_t)mXd;
        *mY = (int32_t)mYd;
    }
}
