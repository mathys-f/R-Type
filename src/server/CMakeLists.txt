add_executable(server)

file(GLOB_RECURSE SERVER_SOURCES "*.cpp")

target_sources(server PRIVATE ${SERVER_SOURCES})

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../game_engine
)

target_link_libraries(server PRIVATE ecs networking game_engine, raylib)

set_target_properties(server PROPERTIES
    OUTPUT_NAME "r-type_server"
)
