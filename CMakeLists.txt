cmake_minimum_required(VERSION 4.1.2)
project(BitForge VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(
    include/
    src/
)

include_directories(SYSTEM
    /usr/include/glad/
)

add_executable(${PROJECT_NAME}
    src/renderer.cpp
    src/glad.c
)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
)

add_custom_command(TARGET BitForge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources
    DESTINATION ${CMAKE_INSTALL_PREFIX}/resources)
