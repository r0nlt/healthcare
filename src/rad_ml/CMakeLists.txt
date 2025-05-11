# File: src/rad_ml/CMakeLists.txt

# Add rad_ml subdirectories
add_subdirectory(sim)
add_subdirectory(research)
add_subdirectory(physics)

# Define the core library - create a static library rather than interface-only
# This ensures there's an actual library file to link against
add_library(rad_ml_core STATIC
    # Core library source files
    # Add more source files as needed
)

# Set include directories for the core library
target_include_directories(rad_ml_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Define neural library (needed by research)
add_library(rad_ml_neural STATIC
    # Neural library source files
    # Add more source files as needed
)

# Set include directories for the neural library
target_include_directories(rad_ml_neural
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Set dependencies for neural library
target_link_libraries(rad_ml_neural
    PUBLIC
        rad_ml_core
) 