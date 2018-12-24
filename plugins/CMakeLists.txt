add_library(plugin1 SHARED plugin1.cpp)
target_include_directories(plugin1 PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(plugin1 PUBLIC
    allegro
    allegro_primitives
    Boost::boost
)

add_library(plugin2 SHARED plugin2.cpp)
target_include_directories(plugin2 PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(plugin2 PUBLIC
    allegro
    allegro_primitives
    Boost::boost
)