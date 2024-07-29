#pragma once

#include "Car/Core/Core.hpp"

namespace Car {
    template <typename T> class PointGeneric {
    public:
        PointGeneric(T x, T y) : x(x), y(y) {}
        PointGeneric() : x(0), y(0) {}

        CR_INLINE void update(T x_, T y_) {
            x = x_;
            y = y_;
        }

    public:
        T x, y;
    };

    using Point = PointGeneric<int>;
    using FPoint = PointGeneric<float>;
    using DPoint = PointGeneric<double>;
} // namespace Car
