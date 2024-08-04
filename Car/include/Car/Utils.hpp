#pragma once

#include "Car/Core/Core.hpp"

namespace Car {
    std::string readFile(const std::string& path);
    std::string readFileBinary(const std::string& path);

    void writeToFile(const std::string& path, const uint8_t* data, size_t size);
} // namespace Car
