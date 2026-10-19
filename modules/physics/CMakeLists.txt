add_library(atom-physics)

target_sources(atom-physics PRIVATE src/C/physics.c)

target_include_directories(atom-physics PUBLIC include)
