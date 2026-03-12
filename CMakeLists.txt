cmake_minimum_required(VERSION 3.21)

project(ATOM VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ATOM_BUILD_DEMOS "Build project-wide demo" OFF)

add_subdirectory(modules/core)
add_subdirectory(modules/audio)
add_subdirectory(modules/inputs)
add_subdirectory(modules/network)
add_subdirectory(modules/physics)
add_subdirectory(modules/renderer)
add_subdirectory(modules/ressources)
add_subdirectory(modules/script)
add_subdirectory(modules/tools)

if(ATOM_BUILD_DEMOS)
    add_subdirectory(demo)
endif()
