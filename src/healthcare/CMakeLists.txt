cmake_minimum_required(VERSION 3.10)

# Set the project name
project(rad_ml_healthcare)

# Add subdirectories if needed
# add_subdirectory(chemotherapy)

# Define library target if there are implementations
# For now, we'll just create an empty header-only interface library
add_library(rad_ml_healthcare INTERFACE)

# Include directories
target_include_directories(rad_ml_healthcare
    INTERFACE
    ${CMAKE_SOURCE_DIR}/include
)

# Set C++17 standard
set_target_properties(rad_ml_healthcare PROPERTIES
    INTERFACE_CXX_STANDARD 17
    INTERFACE_CXX_STANDARD_REQUIRED ON
)
