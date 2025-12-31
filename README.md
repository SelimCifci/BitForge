# Bitforge
(Soon to be) Code-only game engine written in C++ with OpenGL and Assimp for maximized performace (unlike whatever Epic Games is doing)

# Prerequisites

## Cmake
- Install from https://cmake.org/download/ or `sudo pacman -S cmake` for Arch people (I use Arch btw)

## Dependencies
- OpenGL
- Assimp
- Install from https://wikis.khronos.org/opengl/Getting_Started#Downloading_OpenGL and https://github.com/assimp/assimp/releases or `sudo pacman -S mesa assimp`

# Running
## Build
- Linux: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build && cp -r ./resources ./build/`
- Windows: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build && xcopy /E /I /H /Y .\resources .\build\resources\`

## Run
Linux: `./build/BitForge`
Windows: `.\build\BitForge.exe`
