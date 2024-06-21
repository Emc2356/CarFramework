#pragma once

#include "Car/Time.hpp"


namespace Car {
    class GLFWTime : public Time {
    protected:
        virtual double GetImpl() override;
    };
}
