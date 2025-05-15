cmake_minimum_required(VERSION 3.10)
project(rad_tolerant_ml_healthcare VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Create healthcare test executable
add_executable(healthcare_test
    test/healthcare_test.cpp
    src/healthcare/bio_quantum_integration.cpp
)

# Set output directory
set_target_properties(healthcare_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Print configuration summary
message(STATUS "Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Include directory: ${CMAKE_CURRENT_SOURCE_DIR}/include")
