cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_PROJECT_NAME)
    project(atom-core VERSION 0.1.0 LANGUAGES C CXX)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_library(atom-core)

target_sources(atom-core PRIVATE
    src/C/core.c
    src/Cpp/DefaultComponents.cpp
)

target_include_directories(atom-core PUBLIC include)
