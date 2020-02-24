#pragma once

#include <string>
#include <chrono>

namespace rb {
    
    class Clock {
    public:
        Clock();
        
        void Reset();
        float Delta();
        float Duration();
        static long Current();
        
    private:
        bool _firstTime;
        std::chrono::high_resolution_clock::time_point _timestamp;
    };
    
}
