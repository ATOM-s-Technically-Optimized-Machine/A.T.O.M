add_library(atom-network)

target_sources(atom-network PRIVATE src/C/network.c)

target_include_directories(atom-network PUBLIC include)
