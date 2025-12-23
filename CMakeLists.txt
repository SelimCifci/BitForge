cmake_minimum_required(VERSION 4.1.2)
project(BitForge VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

include_directories(
    include/
    bitforge
)

file(GLOB SRC_FILES
     "src/*.cpp"
)

add_executable(${PROJECT_NAME}
    include/glad/glad.c
    include/stb/stb_image.cpp
    bitforge/bitforge.cpp
    bitforge/main.cpp
    ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    assimp
)
