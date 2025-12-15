add_library(game_engine)

file(GLOB_RECURSE GAME_ENGINE_SOURCES "*.cpp")

target_sources(game_engine PRIVATE ${GAME_ENGINE_SOURCES})

target_include_directories(game_engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(game_engine
    PUBLIC ecs glm lua sol2
    PRIVATE lua networking raylib
)
