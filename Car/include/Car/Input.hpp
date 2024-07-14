#pragma once

#include "Car/Core/Core.hpp"


namespace Car::Input {
    bool IsKeyPressed(uint32_t keycode);
    bool IsMouseButtonPressed(int button);
    void MouseX(int32_t* mX);
    void MouseY(int32_t* mY);
    int32_t MouseX();
    int32_t MouseY();
    void MousePos(int32_t* mX, int32_t* mY);
    std::pair<int32_t, int32_t> MousePos();
}
