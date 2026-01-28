add_executable(ecs_tests)

file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

target_sources(ecs_tests PRIVATE ${TEST_SOURCES})

target_include_directories(ecs_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_link_libraries(ecs_tests
    PRIVATE
        ecs
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ecs_tests)
