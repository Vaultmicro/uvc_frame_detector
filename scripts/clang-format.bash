#!/bin/bash

# Apply clang-format to all .cpp files in specified directories

cd ..
clang-format -i source/*.cpp
clang-format -i include/*.hpp
clang-format -i examples/*.cpp
clang-format -i tests/*.cpp
cd scripts

echo "clang-format applied to all .cpp files in source, examples, and tests directories."
