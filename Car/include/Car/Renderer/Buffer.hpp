#pragma once

#include "Car/Core/Core.hpp"

namespace Car {
    // a "namespace-class" that holds enums that are used by various buffers
    class Buffer {
    public:
        enum class Usage {
            None = 0,
            StaticDraw,
            DynamicDraw,
        };
        enum class Type {
            None = 0,
            Float,
            Double,
            UnsignedInt,
            UnsignedShort,
            Byte,
        };
        static uint32_t sizeOfType(Type type);
    };
} // namespace Car
