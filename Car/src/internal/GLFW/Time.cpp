#include "Car/internal/GLFW/Time.hpp"

#include <GLFW/glfw3.h>


namespace Car {
    Time* Time::sInstance = new GLFWTime();

    double GLFWTime::GetImpl() {
        return glfwGetTime();
    }
}
