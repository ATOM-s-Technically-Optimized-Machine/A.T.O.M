add_library(atom-renderer)

target_include_directories(atom-renderer PUBLIC include)

target_link_libraries(atom-renderer PUBLIC atom-core)
