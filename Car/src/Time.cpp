#include "Car/Time.hpp"
#include "Car/Core/Log.hpp"
#include <chrono>


namespace Car {
    uint64_t Time::GetMilli() {
        return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
        
    uint64_t Time::GetMicro() {
        return (uint64_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
            
    uint64_t Time::GetNano() {
        return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
} // namespace Car::Time
