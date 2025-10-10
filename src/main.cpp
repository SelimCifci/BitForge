#include <renderer.hpp>

#include <iostream>

int main()
{
    Renderer renderer(600, 600, "BitForge");
    
    renderer.init();

    while (renderer.getRunStatus())
    {
        renderer.update();
    }

    renderer.terminate();

    return 0;
}
