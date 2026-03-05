add_library(atom-core)

target_sources(atom-core PRIVATE src/C/core.c)

target_include_directories(atom-core PUBLIC include)
