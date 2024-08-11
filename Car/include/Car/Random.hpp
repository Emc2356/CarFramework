#pragma once

#include "Car/Core/Core.hpp"

namespace Car {
    class Random {
    public:
        // automatically called
        static void Init() { sRandomEngine.seed(std::random_device()()); }

        static uint32_t Uint() { return sDistribution(sRandomEngine); }
        static uint32_t UInt(uint32_t min, uint32_t max) {
            return min + (sDistribution(sRandomEngine) % (max - min + 1));
        }
        // static float Float() { return (float)sDistribution(sRandomEngine) /
        // (float)std::numeric_limits<uint32_t>::max(); } static float Float(float min, float max) { return min +
        // Float() * (max - min); } static glm::vec3 Vec3() { return glm::vec3(Float(), Float(), Float()); } static
        // glm::vec3 Vec3(float min, float max) { return glm::vec3(Float() * (max - min) + min, Float() * (max - min) +
        // min, Float() * (max - min) + min); } static glm::vec2 Vec2() { return glm::vec2(Float(), Float()); } static
        // glm::vec2 Vec2(float min, float max) { return glm::vec2(Float() * (max - min) + min, Float() * (max - min) +
        // min); }
        template <typename T> static const T& FromVec(const std::vector<T>& vec) {
            return vec[Random::UInt(0, vec.size())];
        }

    private:
        static std::mt19937 sRandomEngine;
        static std::uniform_int_distribution<std::mt19937::result_type> sDistribution;
    };
} // namespace Car
