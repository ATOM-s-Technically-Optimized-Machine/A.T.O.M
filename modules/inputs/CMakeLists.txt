add_library(atom-inputs)

target_sources(atom-inputs PRIVATE src/C/inputs.c)

target_include_directories(atom-inputs PUBLIC include)
