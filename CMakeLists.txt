cmake_minimum_required(VERSION 3.14)
project(rad_tolerant_ml VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_HEALTHCARE "Build healthcare module" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Find packages
find_package(Threads REQUIRED)

# Setup include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

# Add the src directory which contains our core components
add_subdirectory(src)

# Create the rad_ml_core library
file(GLOB_RECURSE CORE_SOURCES
    "src/rad_ml/core/*.cpp"
)

# If no core sources found, create an interface library
if(NOT CORE_SOURCES)
    add_library(rad_ml_core INTERFACE)
    target_include_directories(rad_ml_core INTERFACE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
    )
else()
    add_library(rad_ml_core ${CORE_SOURCES})
    target_include_directories(rad_ml_core PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
    )
    target_link_libraries(rad_ml_core
        Threads::Threads
    )
endif()

# Add healthcare directory
if(BUILD_HEALTHCARE)
    add_subdirectory(src/healthcare)

    # Create the rad_tolerant_ml_healthcare library
    file(GLOB_RECURSE HEALTHCARE_SOURCES
        "src/rad_ml/healthcare/*.cpp"
    )

    add_library(rad_tolerant_ml_healthcare ${HEALTHCARE_SOURCES})

    target_include_directories(rad_tolerant_ml_healthcare PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
    )

    target_link_libraries(rad_tolerant_ml_healthcare
        Threads::Threads
        rad_ml_core
    )
endif()

# Tests are now handled via src/test
# The original test directory is no longer used
if(BUILD_TESTS)
    enable_testing()
    # No need to add_subdirectory here as src/test is already included via src
endif()

# Build examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
