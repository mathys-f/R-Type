# Enable clang-tidy for all targets in src/ if available
if(CLANG_TIDY_COMMAND)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

# Static libraries
add_subdirectory(ecs)
add_subdirectory(networking)
add_subdirectory(game_engine)

# Executables
add_subdirectory(client)
add_subdirectory(server)
