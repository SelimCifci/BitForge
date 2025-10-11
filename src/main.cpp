#include <renderer.hpp>

int main()
{
    // create a renderer
    Renderer renderer(0, 0, "BitForge", true);
    
    // import all textures to be used
    renderer.importTextures({
        texture("resources/textures/arch.jpg", 0),
        texture("resources/textures/container.jpg", 0)
    });

    // generate Buffers
    renderer.genBuffers();

    // select which texture to use initially
    renderer.setTexture(1);

    // main loop
    while (renderer.getRunStatus())
    {
        renderer.update();
    }

    // terminate program
    renderer.terminate();

    return 0;
}
