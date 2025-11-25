add_executable(server)

file(GLOB SERVER_SOURCES "*.cpp")

target_sources(server PRIVATE ${SERVER_SOURCES})

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(server PRIVATE ecs networking)

set_target_properties(server PROPERTIES
    OUTPUT_NAME "R-Type-server"
)
