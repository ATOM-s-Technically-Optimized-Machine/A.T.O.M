find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# --- Library ---
add_library(atom-renderer)

target_include_directories(atom-renderer PUBLIC include)

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
    atom-core
    atom-resources
    OpenGL::GL
    glfw
    glm::glm
)
