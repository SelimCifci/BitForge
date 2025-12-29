#include <bitforge.hpp>
#include <iostream>

namespace {
    void start()
    {
        std::cout << "started" << std::endl;
    }

    void update(double delta_time)
    {
        //std::cout << "updated, delta time: " << delta_time << std::endl;
    }
}

REGISTER(start, update)
