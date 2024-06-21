#pragma once

#include "Car/Core/Core.hpp"


namespace Car {
    class Input {
    public:
        static bool IsKeyPressed(uint32_t keycode) { return sInstance->IsKeyPressedImpl(keycode); }
        static bool IsMouseButtonPressed(int button) { return sInstance->IsMouseButtonPressedImpl(button); }
        static void MouseX(int32_t* mX) { sInstance->MouseXImpl(mX); }
        static void MouseY(int32_t* mY) { sInstance->MouseYImpl(mY); }
        static int32_t MouseX() { int32_t tmp; sInstance->MouseXImpl(&tmp); return tmp; }
        static int32_t MouseY() { int32_t tmp; sInstance->MouseYImpl(&tmp); return tmp; }
        static void MousePos(int32_t* mX, int32_t* mY) { sInstance->MousePosImpl(mX, mY); }
        static std::pair<int32_t, int32_t> MousePos() { int32_t mX, mY; sInstance->MousePosImpl(&mX, &mY); return std::pair<int32_t, int32_t>(mX, mY); }
    protected:
        virtual bool IsKeyPressedImpl(uint32_t keycode) = 0;
        virtual bool IsMouseButtonPressedImpl(int button) = 0;
        virtual void MouseXImpl(int32_t* mX) = 0;
        virtual void MouseYImpl(int32_t* mY) = 0;
        virtual void MousePosImpl(int32_t* mX, int32_t* mY) = 0;
    private:
        static Car::Input* sInstance;
    };
}
