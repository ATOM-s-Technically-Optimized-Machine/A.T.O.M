cmake_minimum_required(VERSION 3.21)

project(ATOM VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ATOM_MODULES
    core
    audio
    inputs
    network
    physics
    renderer
    ressources
    script
    tools
)

foreach(mod IN LISTS ATOM_MODULES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/modules/${mod}/CMakeLists.txt")
        add_subdirectory("modules/${mod}")
    else()
        message(STATUS "Skipping module '${mod}' — no CMakeLists.txt")
    endif()
endforeach()
