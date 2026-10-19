cmake_minimum_required(VERSION 3.21)
project(atom-inputs VERSION 2.0.0 LANGUAGES CXX)

# ---- C++ Standard ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Standalone guard ----
if(NOT CMAKE_PROJECT_NAME OR CMAKE_PROJECT_NAME STREQUAL "atom-inputs")
    set(ATOM_INPUTS_STANDALONE ON)
else()
    set(ATOM_INPUTS_STANDALONE OFF)
endif()

# ---- Options ----
option(ATOM_INPUTS_BUILD_TESTS "Build unit tests" OFF)
option(ATOM_INPUTS_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ATOM_INPUTS_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ATOM_INPUTS_ENABLE_COVERAGE "Enable code coverage (gcov)" OFF)
option(ATOM_INPUT_USE_GLFW "Enable GLFW bridge" OFF)

# ---- Sanitizers ----
if(ATOM_INPUTS_ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

if(ATOM_INPUTS_ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

# ---- Coverage ----
if(ATOM_INPUTS_ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# ---- Dependencies: GLM ----
find_package(glm QUIET)
if(NOT glm_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(glm)
endif()

# ---- Dependencies: atom-core (for ECS integration) ----
# When built inside A.T.O.M, atom-core is already available as a target.
# When standalone, ECS integration is simply excluded from the build.

# ---- Dependencies: GLFW (optional) ----
if(ATOM_INPUT_USE_GLFW)
    find_package(glfw3 QUIET)
    if(NOT glfw3_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        3.4
            GIT_SHALLOW    TRUE
        )
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(glfw)
    endif()
endif()

# ---- Main library ----
add_library(atom-inputs STATIC)

set(ATOM_INPUTS_SOURCES
    src/Cpp/Event.cpp
    src/Cpp/InputState.cpp
    src/Cpp/Camera.cpp
    src/Cpp/FPSCamera.cpp
    src/Cpp/OrbitalCamera.cpp
    src/Cpp/InputSystem.cpp
)

# ECS integration (requires atom-core)
if(TARGET atom-core)
    list(APPEND ATOM_INPUTS_SOURCES src/Cpp/ECSIntegration.cpp)
endif()

# GLFW bridge (conditional)
if(ATOM_INPUT_USE_GLFW)
    list(APPEND ATOM_INPUTS_SOURCES src/Cpp/GLFWBridge.cpp)
endif()

target_sources(atom-inputs PRIVATE ${ATOM_INPUTS_SOURCES})

target_include_directories(atom-inputs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ---- Link dependencies ----
target_link_libraries(atom-inputs PUBLIC glm::glm)

if(TARGET atom-core)
    target_link_libraries(atom-inputs PUBLIC atom-core)
endif()

if(ATOM_INPUT_USE_GLFW)
    target_link_libraries(atom-inputs PRIVATE glfw)
    target_compile_definitions(atom-inputs PUBLIC ATOM_INPUT_HAS_GLFW=1)
endif()

# ---- Compiler warnings ----
target_compile_options(atom-inputs PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# ---- Tests ----
if(ATOM_INPUTS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---- Export for parent project ----
set(ATOM_INPUTS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
