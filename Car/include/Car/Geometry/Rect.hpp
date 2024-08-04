#pragma once

#include "Car/Core/Core.hpp"

namespace Car {

    class Rect {
    public:
        Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
        Rect(glm::vec2 pos, glm::vec2 size) : x(pos.x), y(pos.y), w(size.x), h(size.y) {}
        Rect() : x(0), y(0), w(0), h(0) {}

        CR_INLINE void update(float x_, float y_, float w_, float h_) {
            x = x_;
            y = y_;
            w = w_;
            h = h_;
        }

        CR_INLINE void move(float xOffset, float yOffset) {
            x += xOffset;
            y += yOffset;
        }

        CR_INLINE bool contains(float pX, float pY) const { return (pX >= x && pX <= x + w && pY >= y && pY <= y + h); }
        CR_INLINE bool contains(glm::vec2 p) const { return (p.x >= x && p.x <= x + w && p.y >= y && p.y <= y + h); }

        CR_INLINE glm::vec2 topleft() const { return {x, y}; }
        CR_INLINE glm::vec2 topmid() const { return {x + w / h, y}; }
        CR_INLINE glm::vec2 topright() const { return {x + w, y}; }
        CR_INLINE glm::vec2 rightmid() const { return {x + w, y + h / 2}; }
        CR_INLINE glm::vec2 bottomright() const { return {x + w, y + h}; }
        CR_INLINE glm::vec2 bottommid() const { return {x + w / 2, y + h}; }
        CR_INLINE glm::vec2 bottomleft() const { return {x, y + h}; }
        CR_INLINE glm::vec2 leftmid() const { return {x, y + h / 2}; }
        CR_INLINE glm::vec2 center() const { return {x + w / 2, y + h / 2}; }

    public:
        float x, y;
        float w, h;
    };
} // namespace Car
