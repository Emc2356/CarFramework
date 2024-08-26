#pragma once

#include "Car/Core/Core.hpp"

namespace Car {
    class Time {
    public:
        static uint64_t GetMilli();
        static uint64_t GetMicro();
        static uint64_t GetNano();
    };
} // namespace Car
