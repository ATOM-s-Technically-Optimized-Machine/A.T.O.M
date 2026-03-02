add_library(atom-ressources)

target_sources(atom-ressources PRIVATE src/C/ressources.c)

target_include_directories(atom-ressources PUBLIC include)
