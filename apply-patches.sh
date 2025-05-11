#!/bin/bash
set -e

# Apply patch to pybind11 CMakeLists.txt to fix cmake version
echo "Applying pybind11 CMake version patch..."
patch -p0 < patches/pybind11-cmake-version.patch

echo "Patches applied successfully!" 