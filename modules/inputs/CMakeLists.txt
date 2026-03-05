cmake_minimum_required(VERSION 3.16)
project(atom-inputs VERSION 1.0.0 LANGUAGES C)

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Main library
add_library(atom-inputs STATIC)

target_sources(atom-inputs PRIVATE
    src/C/inputs.c
    src/C/atom_math.c
    src/C/input_event.c
    src/C/input_state.c
    src/C/camera.c
    src/C/camera_fps.c
    src/C/camera_orbital.c
)

target_include_directories(atom-inputs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link math library on Unix
if(UNIX)
    target_link_libraries(atom-inputs PUBLIC m)
endif()

# Compiler warnings
target_compile_options(atom-inputs PRIVATE
    $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<C_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<C_COMPILER_ID:MSVC>:/W4>
)

# Export for use as submodule
set(ATOM_INPUTS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
