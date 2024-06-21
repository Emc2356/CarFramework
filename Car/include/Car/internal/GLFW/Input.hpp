#pragma once

#include "Car/Input.hpp"


namespace Car {
    class GLFWInput : public Input {
    protected:
        virtual bool IsKeyPressedImpl(uint32_t keycode) override;
        virtual bool IsMouseButtonPressedImpl(int button) override;
        virtual void MouseXImpl(int32_t* mX) override;
        virtual void MouseYImpl(int32_t* mY) override;
        virtual void MousePosImpl(int32_t* mX, int32_t* mY) override;
    };
}
