cmake_minimum_required(VERSION 3.10)
project(quantum_wave_mc_test)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Threads package
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../
    ${CMAKE_SOURCE_DIR}/../include
)

# Source files
set(SOURCES
    ../src/test/quantum_wave_mc_test.cpp
    ../src/physics/quantum_integration.cpp
    ../src/rad_ml/physics/quantum_models.cpp
)

# Create executable
add_executable(quantum_wave_mc_test ${SOURCES})

# Link with threads
target_link_libraries(quantum_wave_mc_test Threads::Threads)
