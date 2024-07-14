#include "Car/Input.hpp"
#include "Car/Application.hpp"

#include <GLFW/glfw3.h>


namespace Car::Input {
    bool IsKeyPressed(uint32_t keycode) {
        GLFWwindow* window = Application::Get()->getWindow()->getWindowHandle();

        int state = glfwGetKey(window, keycode);

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool IsMouseButtonPressed(int button) {
        GLFWwindow* window = Application::Get()->getWindow()->getWindowHandle();

        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    void MouseX(int32_t* mX) {
        GLFWwindow* window = Application::Get()->getWindow()->getWindowHandle();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        *mX = (int32_t)mXd;

        UNUSED(mYd);
    }

    int32_t MouseX() {
        GLFWwindow* window = Application::Get()->getWindow()->getWindowHandle();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        return (int32_t)mXd;
    }

    void MouseY(int32_t* mY)  {
        GLFWwindow* window = Application::Get()->getWindow()->getWindowHandle();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        *mY = (int32_t)mYd;

        UNUSED(mXd);
    }

    int32_t MouseY()  {
        GLFWwindow* window = Application::Get()->getWindow()->getWindowHandle();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        return (int32_t)mYd;
    }

    void MousePos(int32_t* mX, int32_t* mY)  {
        GLFWwindow* window = Application::Get()->getWindow()->getWindowHandle();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        *mX = (int32_t)mXd;
        *mY = (int32_t)mYd;
    }

    std::pair<int32_t, int32_t> MousePos()  {
        GLFWwindow* window = Application::Get()->getWindow()->getWindowHandle();

        double mXd, mYd;

        glfwGetCursorPos(window, &mXd, &mYd);

        return std::pair<int32_t, int32_t>((int32_t)mXd, (int32_t)mYd);
    }
}
