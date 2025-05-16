cmake_minimum_required(VERSION 3.10)

# Set the project name
project(rad_ml_tests)

# Find Threads package for parallel processing
find_package(Threads REQUIRED)

# Add the quantum wave Monte Carlo test
add_executable(quantum_wave_mc_test
    quantum_wave_mc_test.cpp
)

# Link against required libraries
target_link_libraries(quantum_wave_mc_test
    rad_ml_physics
    rad_ml_core
    Threads::Threads
)

# Set C++17 standard
set_target_properties(quantum_wave_mc_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Add test runner executable
add_executable(test_runner
    test_runner.cpp
)

# Link against required libraries
target_link_libraries(test_runner
    rad_ml_physics
    rad_ml_core
    Threads::Threads
)

# Set C++17 standard for test runner
set_target_properties(test_runner PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Add other test executables as needed
