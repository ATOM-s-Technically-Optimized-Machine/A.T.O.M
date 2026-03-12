add_executable(atom-demo main.cpp)

target_link_libraries(atom-demo PRIVATE
    atom-renderer
    atom-resources
)
