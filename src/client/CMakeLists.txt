add_executable(client)

file(GLOB CLIENT_SOURCES "*.cpp")

target_sources(client PRIVATE ${CLIENT_SOURCES})

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(client PRIVATE ecs networking raylib game_engine)

set_target_properties(client PROPERTIES
    OUTPUT_NAME "r-type_client"
)
