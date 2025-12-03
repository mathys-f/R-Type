add_library(utils INTERFACE)

file(GLOB UTILS_SOURCES "*.h")

target_sources(utils INTERFACE ${UTILS_SOURCES})

target_include_directories(utils INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(utils INTERFACE
    glm
)
