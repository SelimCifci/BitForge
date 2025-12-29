#include <bitforge.hpp>
#include <iostream>

namespace {
    void start()
    {
        
    }

    void update(double delta_time)
    {
        std::cout << "fps: " << (int)(1.0 / delta_time) << std::endl;
    }
}

REGISTER(start, update)