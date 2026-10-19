add_library(atom-script)

target_sources(atom-script PRIVATE src/C/script.c)

target_include_directories(atom-script PUBLIC include)
