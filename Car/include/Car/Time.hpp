#pragma once

#include "Car/Core/Core.hpp"


namespace Car {
    class Time {
    public:
        static double Get() { return sInstance->GetImpl(); }
    protected:
        virtual double GetImpl() = 0;
    private:
        static Car::Time* sInstance;
    };
}
