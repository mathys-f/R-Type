add_library(ecs)

file(GLOB ECS_SOURCES "*.cpp")

target_sources(ecs PRIVATE ${ECS_SOURCES})

target_include_directories(ecs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
