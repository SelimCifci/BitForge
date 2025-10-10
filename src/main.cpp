#include <renderer.hpp>

#include <iostream>

int main()
{
    Renderer renderer(0, 0, "BitForge", true);

    while (renderer.getRunStatus())
    {
        renderer.update();
    }

    renderer.terminate();

    return 0;
}
