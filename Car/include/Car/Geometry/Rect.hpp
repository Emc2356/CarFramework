#pragma once

#include "Car/Core/Core.hpp"

#include "Car/Geometry/Point.hpp"


namespace Car {
    
    template<typename T>
    class RectGeneric {
    public:
        RectGeneric(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {}
        RectGeneric() : x(0), y(0), w(0), h(0) {}

        CR_INLINE void update(T x_, T y_, T w_, T h_) { x = x_; y = y_; w = w_; h = h_; }

        CR_INLINE void move(T xOffset, T yOffset) { x += xOffset; y += yOffset; }

        CR_INLINE bool contains(T pX, T pY) const { return (pX >= x && pX <= x + w &&  pY >= y && pY <= y + h); }
        CR_INLINE bool contains(PointGeneric<T> p) const { return (p.x >= x && p.x <= x + w &&  p.y >= y && p.y <= y + h); }

        CR_INLINE PointGeneric<T> topleft() const { return {x, y}; }
        CR_INLINE PointGeneric<T> topmid() const { return {x + w / h, y}; }
        CR_INLINE PointGeneric<T> topright() const { return {x + w, y}; }
        CR_INLINE PointGeneric<T> rightmid() const { return {x + w, y + h / 2}; }
        CR_INLINE PointGeneric<T> bottomright() const { return {x + w, y + h}; }
        CR_INLINE PointGeneric<T> bottommid() const { return {x + w / 2, y + h}; }
        CR_INLINE PointGeneric<T> bottomleft() const { return {x, y + h}; }
        CR_INLINE PointGeneric<T> leftmid() const { return {x, y + h / 2}; }
        CR_INLINE PointGeneric<T> center() const { return {x + w / 2, y + h / 2}; }
    public:
        T x, y;
        T w, h;
    };


    using Rect = RectGeneric<int32_t>;
    using FRect = RectGeneric<float>;
    using DRect = RectGeneric<double>;
}
