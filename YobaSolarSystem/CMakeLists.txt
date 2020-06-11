cmake_minimum_required(VERSION 3.16)
project(Sfml)

set(CMAKE_CXX_STANDARD 17)
include_directories("D:\\Link\\SFML-2.5.1-vs2019\\include")
include_directories("D:\\Link\\opengl\\include")
include_directories("D:\\Link\\imgui\\include")
link_directories("D:\\Link\\SFML-2.5.1-vs2019\\lib")
link_directories("D:\\Link\\opengl\\lib")
add_executable(Sfml main.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui-SFML.cpp
        imgui/imgui.cpp Math.cpp Math.h CelestialObject.cpp CelestialObject.h World.cpp World.h)
target_link_libraries(Sfml
        sfml-window-d
        sfml-graphics-d
        sfml-network-d
        sfml-system-d
        sfml-main-d
        sfml-audio-d
        glut32
        glew32
        glfw3
        glut
        opengl32)
