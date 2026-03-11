cmake_minimum_required(VERSION 3.16)
if(NOT CMAKE_PROJECT_NAME)
    project(atom-resources VERSION 0.1.0 LANGUAGES CXX C)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

add_library(atom-resources STATIC
    src/C/resources.c
    src/Cpp/ResourceLoaders.cpp
    src/libs/C/stb_image.c
    src/libs/C/dr_wav.c
    src/libs/Cpp/tiny_obj_loader.cpp
)

target_include_directories(atom-resources PUBLIC include)
target_include_directories(atom-resources PRIVATE src/vendor)
