#!/bin/bash

# Apply clang-format to all .cpp files in specified directories
clang-format -i source/*.cpp
clang-format -i examples/*.cpp
clang-format -i tests/*.cpp

echo "clang-format applied to all .cpp files in source, examples, and tests directories."
    