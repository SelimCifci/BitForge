#include <renderer.hpp>

#include <iostream>

int main()
{
    std::cout << "E\n";
    Renderer renderer(600, 600, "resources/shaders/default.vert", "resources/shaders/default.frag", "BitForge");
    std::cout << "D\n";
    
    renderer.init();
    std::cout << "B\n";

    while (renderer.getRunStatus())
    {
        renderer.update();
    }

    renderer.terminate();
    std::cout << "C\n";

    return 0;
}
