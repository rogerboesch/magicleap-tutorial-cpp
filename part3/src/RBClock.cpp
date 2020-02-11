#include "RBClock.hpp"

#include <chrono>

namespace rb {
    
    Clock::Clock() {
        Reset();
    }

    void Clock::Reset() {
        _firstTime = true;
        Delta();
    }

    float Clock::Delta() {
        if (_firstTime) {
            _timestamp = std::chrono::high_resolution_clock::now();
            _firstTime = false;

            return 0.0f;
        }

        std::chrono::duration<float> fs = std::chrono::high_resolution_clock::now() - _timestamp;
        _timestamp = std::chrono::high_resolution_clock::now();


        return fs.count();
    }

    float Clock::Duration() {
        std::chrono::duration<float> fs = std::chrono::high_resolution_clock::now() - _timestamp;

        return fs.count();
    }

    long Clock::Current() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        return (long)now.time_since_epoch().count();
    }

}
