# Patches

This directory contains patches for external dependencies.

## pybind11-cmake-version.patch

This patch updates the minimum required CMake version in pybind11 from 3.4 to 3.5.

### Issue

Modern CMake versions (≥ 3.27) have deprecated support for CMake versions below 3.5. Without this patch, building the project with recent CMake versions fails with an error:

```
CMake Error at _deps/pybind11-src/CMakeLists.txt:8 (cmake_minimum_required):
  cmake_minimum_required VERSION "3.4" is no longer supported by CMake.  The
  oldest version supported by this running CMake is "3.5".
```

### Solution

The patch increases the minimum required CMake version to 3.5, which maintains compatibility with modern CMake versions while still supporting a broad range of systems.

### Application

To apply this patch, run the `apply-patches.sh` script in the project root:

```bash
./apply-patches.sh
``` 