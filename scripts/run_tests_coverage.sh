#!/bin/bash

BUILD_DIR="build"

echo "Configuring CMake with coverage support..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCOVERAGE=ON

echo "Building networking_tests..."
cmake --build "$BUILD_DIR" --target networking_tests
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

echo "Cleaning old coverage data..."
find "$BUILD_DIR" -name "*.gcda" -delete

echo "Running tests..."
./$BUILD_DIR/bin/Debug/networking_tests
TEST_EXIT_CODE=$?

if [ $TEST_EXIT_CODE -ne 0 ]; then
    echo "Tests failed with exit code $TEST_EXIT_CODE"
    exit 1
fi

echo "Tests passed. Generating coverage report..."
gcovr --root . --filter src/networking --exclude-unreachable-branches --exclude-throw-branches --print-summary
