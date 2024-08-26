#include "Car/Utils.hpp"
#include <stdexcept>

namespace Car {
    std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + path);
        }
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        std::string buffer(size, ' ');
        file.seekg(0);
        file.read(&buffer[0], size);
        return buffer;
    }

    std::string readFileBinary(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + path);
        }
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        std::string buffer(size, ' ');
        file.seekg(0);
        file.read(&buffer[0], size);
        return buffer;
    }

    void writeToFile(const std::string& path, const uint8_t* data, size_t size) {
        std::ofstream wf(path, std::ios::out | std::ios::trunc);

        if (!wf) {
            throw std::runtime_error("failed to open file: " + path);
        }

        wf.write(reinterpret_cast<const char*>(data), size);

        wf.close();

        if (!wf.good()) {
            throw std::runtime_error("error occured at writing time! " + path);
        }
    }
    
    void writeToFile(const std::string& path, const std::vector<uint8_t>& data) {
        std::ofstream wf(path, std::ios::out | std::ios::trunc);
    
        if (!wf) {
            throw std::runtime_error("failed to open file: " + path);
        }

        wf.write(reinterpret_cast<const char*>(data.data()), data.size());

        wf.close();

        if (!wf.good()) {
            throw std::runtime_error("error occured at writing time! " + path);
        }
    }

} // namespace Car
