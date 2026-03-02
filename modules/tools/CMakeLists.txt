add_library(atom-tools)

target_sources(atom-tools PRIVATE src/C/tools.c)

target_include_directories(atom-tools PUBLIC include)
