add_executable(atom-demo main.cpp)

target_compile_definitions(atom-demo PRIVATE
    ATOM_ASSET_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(atom-demo PRIVATE
    atom-renderer
    atom-resources
)
