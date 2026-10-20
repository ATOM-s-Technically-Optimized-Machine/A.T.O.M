cmake_minimum_required(VERSION 3.24)

project(ATOM VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Output layout ----
# Every executable (atom-editor, atom-demo, ...) lands in <build-dir>/bin/,
# e.g. build/dev/bin/atom-editor, instead of deep inside the module tree.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- Common dependencies ----
include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS NAMES glfw3
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm glfw)

# ---- Modules ----
# atom-editor (modules/tools) drives the inputs module through its GLFW bridge,
# so the bridge is always built. Must be set before modules/inputs is added.
set(ATOM_INPUT_USE_GLFW ON)

set(ATOM_MODULES
    core
    resources
    audio
    inputs
    network
    physics
    renderer
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

# ---- Demos ----
option(ATOM_BUILD_DEMOS "Build project-wide demo" OFF)

if(ATOM_BUILD_DEMOS)
    add_subdirectory(demo)
endif()
