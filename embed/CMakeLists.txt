add_library(embed STATIC embed.cpp embed.h)
target_include_directories(embed PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(embeder embeder.cpp)
target_link_libraries(embeder PUBLIC
        embed
        clara)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resource.cpp
    COMMAND embeder -i ${CMAKE_CURRENT_SOURCE_DIR}/embeder.cpp -o ${CMAKE_CURRENT_BINARY_DIR}/resource.cpp -n resource
    DEPENDS embeder ${CMAKE_CURRENT_SOURCE_DIR}/embeder.cpp)


add_executable(embed_test embed_test.cpp ${CMAKE_CURRENT_BINARY_DIR}/resource.cpp)
target_include_directories(embed_test PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(embed_test PUBLIC
        embed)