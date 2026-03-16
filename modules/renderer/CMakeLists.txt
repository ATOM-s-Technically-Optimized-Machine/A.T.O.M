cmake_minimum_required(VERSION 3.21)

if(NOT CMAKE_PROJECT_NAME)
    project(atom-renderer VERSION 0.1.0 LANGUAGES C CXX)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# ---- Options ----
option(ATOM_RENDERER_SANDBOX "Build sandbox executable" OFF)

# ---- Dependencies: OpenGL ----
find_package(OpenGL REQUIRED)

# ---- Dependencies: GLFW ----
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

# ---- Dependencies: atom-core and atom-resources (for standalone CI builds) ----
if(NOT TARGET atom-core AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/module-core/CMakeLists.txt")
    add_subdirectory(deps/module-core)
endif()
if(NOT TARGET atom-resources AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/module-ressources/CMakeLists.txt")
    add_subdirectory(deps/module-ressources)
endif()

# ---- Library ----
add_library(atom-renderer)

target_include_directories(atom-renderer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_sources(atom-renderer PRIVATE
    src/libs/C/stb_image.c
    src/libs/Cpp/tiny_obj_loader.cpp
    src/C/renderer.c
    src/C/glad/glad.c
    src/Cpp/RenderSystem.cpp
    src/Cpp/Renderer.cpp
    src/Cpp/GLFWInputProvider.cpp
)

target_link_libraries(atom-renderer PUBLIC
    atom-inputs
    OpenGL::GL
    glfw
    glm::glm
)

if(TARGET atom-core)
    target_link_libraries(atom-renderer PUBLIC atom-core)
endif()

if(TARGET atom-resources)
    target_link_libraries(atom-renderer PUBLIC atom-resources)
endif()

# ---- Compiler warnings ----
target_compile_options(atom-renderer PRIVATE
    $<$<C_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<C_COMPILER_ID:Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# ---- Export for parent project ----
set(ATOM_RENDERER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
