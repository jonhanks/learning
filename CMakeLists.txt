cmake_minimum_required(VERSION 3.12)
project(learning)

set(CMAKE_CXX_STANDARD 14)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Allegro5)
find_package(Boost REQUIRED COMPONENTS filesystem system)

add_subdirectory(external)
add_subdirectory(embed)
add_subdirectory(gui)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/allegro_rsc.cpp
    COMMAND embeder -i ${CMAKE_CURRENT_SOURCE_DIR}/include/allegro.hh -o ${CMAKE_CURRENT_BINARY_DIR}/allegro_rsc.cpp -n allegro
    DEPENDS embeder ${CMAKE_CURRENT_SOURCE_DIR}/include/allegro.hh
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/plugin_rsc.cpp
        COMMAND embeder -i ${CMAKE_CURRENT_SOURCE_DIR}/include/plugin.h -o ${CMAKE_CURRENT_BINARY_DIR}/plugin_rsc.cpp -n plugin
        DEPENDS embeder ${CMAKE_CURRENT_SOURCE_DIR}/include/plugin.h
)

add_executable(learning main.cpp
    include/allegro.hh
    include/plugin.h
    ${CMAKE_CURRENT_BINARY_DIR}/allegro_rsc.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/plugin_rsc.cpp
)
target_include_directories(learning PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(learning PUBLIC
    allegro_main
    allegro
    allegro_primitives
    Boost::boost
    Boost::filesystem
    Boost::system
    clara
    dl
    embed
)

add_subdirectory(plugins)


#add_executable(fireworks
#        "main.cc"
#        "include/allegro.hh"
#        )
#target_include_directories(fireworks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_link_libraries(fireworks PUBLIC allegro_main allegro allegro_primitives)
