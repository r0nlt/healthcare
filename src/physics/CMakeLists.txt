cmake_minimum_required(VERSION 3.10)

# Set the project name
project(rad_ml_physics)

# Create a library for the physics module
add_library(rad_ml_physics
    quantum_integration.cpp
)

# Include directories
target_include_directories(rad_ml_physics
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Set C++17 standard
set_target_properties(rad_ml_physics PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Link against other libraries if needed
target_link_libraries(rad_ml_physics
    rad_ml_core
)
