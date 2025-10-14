cmake_minimum_required(VERSION 3.16)
project(graphics_seminar_1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW
add_subdirectory(glfw)

# GLAD
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# Executable
add_executable(main main.cpp
        util.h)
target_include_directories(main PRIVATE glad/include glfw/include)
target_link_libraries(main PRIVATE glad glfw)
