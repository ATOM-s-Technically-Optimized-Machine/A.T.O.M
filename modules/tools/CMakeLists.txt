# ---- Dependencies: OpenGL ----
find_package(OpenGL REQUIRED)

# ---- Dependencies: ImGui ----
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

# ---- atom-tools library ----
add_library(atom-tools)
target_sources(atom-tools PRIVATE src/C/tools.c)
target_include_directories(atom-tools PUBLIC include)

if(TARGET atom-core)
    target_link_libraries(atom-tools PUBLIC atom-core)
endif()
if(TARGET atom-renderer)
    target_link_libraries(atom-tools PUBLIC atom-renderer)
endif()
if(TARGET atom-inputs)
    target_link_libraries(atom-tools PUBLIC atom-inputs)
endif()
if(TARGET atom-resources)
    target_link_libraries(atom-tools PUBLIC atom-resources)
endif()

# ---- atom-editor executable ----
add_executable(atom-editor
    src/Cpp/main.cpp
    src/Cpp/Editor.cpp
    src/Cpp/window/AssetTreeWindow.cpp
)
target_include_directories(atom-editor PRIVATE include)
target_link_libraries(atom-editor PRIVATE atom-tools imgui_lib)
