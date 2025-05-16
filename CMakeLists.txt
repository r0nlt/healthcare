cmake_minimum_required(VERSION 3.14)
project(rad_tolerant_ml VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
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

# Create the rad_tolerant_ml_healthcare library
if(BUILD_HEALTHCARE)
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
    )
endif()

# Build tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Build examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
