#!/bin/bash

echo "Running clang-tidy on all .cpp files under src/ ..."

# Accumulate errors count to report summary
error_count=0

# Loop over all .cpp files
find src/ -type f -name "*.cpp" | while read -r file; do
    echo "Linting $file"
    # Run clang-tidy with standard C++17, suppress system headers warnings
    output=$(clang-tidy "$file" -- -std=c++20 2>&1)
    # Print output
    echo "$output"
    # Check if output contains "error:" to count errors
    if echo "$output" | grep -q "error:"; then
        error_count=$((error_count + 1))
    fi
done

echo "clang-tidy completed. Files with errors: $error_count"
