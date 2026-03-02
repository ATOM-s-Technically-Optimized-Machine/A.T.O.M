add_library(atom-audio)

target_sources(atom-audio PRIVATE src/C/audio.c)

target_include_directories(atom-audio PUBLIC include)
