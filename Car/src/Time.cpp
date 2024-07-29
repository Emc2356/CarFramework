#include "Car/Time.hpp"

#include <GLFW/glfw3.h>

namespace Car::Time {
    double Get() { return glfwGetTime(); }
} // namespace Car::Time
