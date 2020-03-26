cmake_minimum_required(VERSION 3.15)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        include
        lib/glew-2.1.0/include
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/glfw-3.3.2)
add_subdirectory(lib/fmt-6.1.2)
add_subdirectory(lib/glew-2.1.0/build/cmake)

add_executable(
        chip8_emulator WIN32
        include/Operations.h
        src/Operations.cpp
        resources/chip8.rc
        src/Window.cpp
        src/Emulator.cpp)

set_target_properties(chip8_emulator PROPERTIES
        LINK_FLAGS /SUBSYSTEM:CONSOLE
        )

target_link_libraries(chip8_emulator glew)

target_link_libraries(chip8_emulator glfw)

find_package(OpenGL 4.6 REQUIRED)
target_link_libraries(chip8_emulator OpenGL::GL)

target_link_libraries(chip8_emulator fmt)