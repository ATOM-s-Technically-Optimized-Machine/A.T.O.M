find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# --- Library ---
add_library(atom-renderer)

target_include_directories(atom-renderer PUBLIC include)

target_sources(atom-renderer PRIVATE
    src/C/renderer.c
    src/C/glad/glad.c
)

target_link_libraries(atom-renderer PUBLIC
    atom-core
    OpenGL::GL
    glfw
    glm::glm
)

# --- Sandbox executable (dev/test only) ---
if(ATOM_RENDERER_SANDBOX)
    add_executable(atom-renderer-sandbox src/Cpp/renderer.cpp)
    target_link_libraries(atom-renderer-sandbox PRIVATE atom-renderer)
endif()
