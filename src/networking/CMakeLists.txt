add_library(networking)

file(GLOB NETWORKING_SOURCES "*.cpp")

target_sources(networking PRIVATE ${NETWORKING_SOURCES})

target_include_directories(networking PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
