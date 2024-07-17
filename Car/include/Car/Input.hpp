#pragma once

#include "Car/Core/Core.hpp"
#include <glm/glm.hpp>


namespace Car::Input {
    bool IsKeyPressed(uint32_t keycode);
    bool IsMouseButtonPressed(int button);
    void MouseX(int32_t* mX);
    void MouseY(int32_t* mY);
    int32_t MouseX();
    int32_t MouseY();
    void MousePos(int32_t* mX, int32_t* mY);
    glm::ivec2 MousePos();
}
