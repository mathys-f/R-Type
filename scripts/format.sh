#!/bin/bash

echo "Running clang-format on all source files under src/ ..."

# Find all .cpp, .h, .hpp, .tcc files and format them in place
find src/ -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.tcc" \) | while read -r file; do
    echo "Formatting $file"
    clang-format -i "$file"
done

echo "clang-format completed."
