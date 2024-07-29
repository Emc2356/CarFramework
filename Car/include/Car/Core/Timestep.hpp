#pragma once

namespace Car {
    class Timestep {
    public:
        Timestep(double time = 0.0) : mTime(time) {}

        double getSeconds() const { return mTime; }
        double getMilli() const { return mTime * 1000; }

        operator double() const { return mTime; }

    private:
        double mTime;
    };
} // namespace Car
