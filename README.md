# Radiation-Tolerant Machine Learning Framework

A C++ framework for implementing machine learning models that can operate reliably in radiation environments, such as space.

## Features

- Zero dynamic memory allocation
- Triple Modular Redundancy (TMR) for fault tolerance
- Memory scrubbing and error correction
- Fixed-point arithmetic for deterministic execution
- Branchless operations for predictable code paths
- Optimized for low-latency inference

## Getting Started

### Prerequisites

- Modern C++ compiler with C++17 support
- CMake 3.15 or higher

### Building

```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests

```bash
make test
```

## Project Structure

- `include/rad_ml/`: Public headers
- `src/`: Implementation files
- `examples/`: Example applications
- `test/`: Unit and integration tests
- `docs/`: Documentation

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- NASA's radiation effects research
- ESA's fault-tolerant computing guidelines 


## Progress
You've successfully set up:
Core Framework Components:
Memory management (static_allocator.hpp)
Fault tolerance (tmr.hpp)
Error detection/correction (memory_scrubbing.hpp)
Deterministic math (fixed_point.hpp, branchless_ops.hpp)
Example Implementation:
A simple neural network demonstrating TMR and fixed-point arithmetic
Testing Infrastructure:
Basic unit tests for TMR implementation
Build System:
CMake configuration for cross-platform building
Development Infrastructure (just completed):
Code formatting (.clang-format)
Static analysis (.clang-tidy)
Editor configuration (.editorconfig)
Build presets (CMakePresets.json)
IDE settings (.cursor/settings.json)
Git hooks for code quality
You have a working foundation that demonstrates the core principles of radiation-tolerant software. The project can now be expanded with more neural network layers, additional fault-tolerance mechanisms, more comprehensive tests, and additional examples demonstrating space-grade software techniques.