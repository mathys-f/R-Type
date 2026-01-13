add_executable(server)

find_package(Boost 1.84 REQUIRED COMPONENTS date_time)

file(GLOB_RECURSE SERVER_SOURCES "*.cpp")

target_sources(server PRIVATE ${SERVER_SOURCES})

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../game_engine
)

target_link_libraries(server PRIVATE ecs networking game_engine raylib glm lua sol2 Boost::interprocess)

target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})

set_target_properties(server PROPERTIES
    OUTPUT_NAME "r-type_server"
)
