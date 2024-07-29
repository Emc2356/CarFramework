#include "Car/Renderer/Buffer.hpp"

namespace Car {
    uint32_t Buffer::sizeOfType(Buffer::Type type) {
        switch (type) {
        case Buffer::Type::Float: {
            return 4;
        }
        case Buffer::Type::Double: {
            return 8;
        }
        case Buffer::Type::UnsignedInt: {
            return 4;
        }
        case Buffer::Type::UnsignedShort: {
            return 2;
        }
        case Buffer::Type::Byte: {
            return 1;
        }
        default: {
            std::runtime_error("Unrecognized type: " + std::to_string((int)type));
            return 0;
        }
        }
    }
} // namespace Car
