#!/bin/bash

BUILD_DIR="build"

echo "Configuring CMake with coverage support..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCOVERAGE=ON

echo "Building networking_tests and ecs_tests..."
cmake --build "$BUILD_DIR" --target networking_tests ecs_tests
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

echo "Cleaning old coverage data..."
find "$BUILD_DIR" -name "*.gcda" -delete

echo "Running networking tests..."
./$BUILD_DIR/bin/Debug/networking_tests
NETWORKING_EXIT_CODE=$?

if [ $NETWORKING_EXIT_CODE -ne 0 ]; then
    echo "Networking tests failed with exit code $NETWORKING_EXIT_CODE"
    exit 1
fi

echo "Running ecs tests..."
./$BUILD_DIR/bin/Debug/ecs_tests
ECS_EXIT_CODE=$?

if [ $ECS_EXIT_CODE -ne 0 ]; then
    echo "ECS tests failed with exit code $ECS_EXIT_CODE"
    exit 1
fi

echo "All tests passed. Generating coverage report..."
gcovr --root . --filter src/networking --filter src/ecs --exclude ".*\.tcc$" --exclude-unreachable-branches --exclude-throw-branches --print-summary
