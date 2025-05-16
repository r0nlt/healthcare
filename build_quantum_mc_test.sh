#!/bin/bash

# Simple script to build and run the quantum wave Monte Carlo test
# This script avoids the complexity of the full build system

# Colors for better readability
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Create a dedicated build directory for our test
echo -e "${BLUE}Creating dedicated build directory for quantum MC test...${NC}"
mkdir -p quantum_mc_build
cd quantum_mc_build

# Prepare a minimal CMakeLists.txt for our test
echo -e "${BLUE}Creating minimal CMakeLists.txt...${NC}"
cat > CMakeLists.txt << 'EOL'
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
EOL

# Run CMake
echo -e "${BLUE}Configuring with CMake...${NC}"
cmake .

# Build the test
echo -e "${BLUE}Building quantum_wave_mc_test...${NC}"
cmake --build .

# Check if build succeeded
if [ ! -f ./quantum_wave_mc_test ]; then
    echo -e "${YELLOW}Build failed! Executable not found.${NC}"
    echo -e "${YELLOW}Error details:${NC}"
    cat CMakeFiles/CMakeError.log || echo "Error log not available."
    cd ..
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"

# Default parameters for the test
SAMPLES=10000
THREADS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
TEMP_MIN=10.0
TEMP_MAX=300.0
SIZE_MIN=2.0
SIZE_MAX=50.0
BARRIER_MIN=0.1
BARRIER_MAX=5.0

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --samples)
            SAMPLES="$2"
            shift 2
            ;;
        --threads)
            THREADS="$2"
            shift 2
            ;;
        --temp-min)
            TEMP_MIN="$2"
            shift 2
            ;;
        --temp-max)
            TEMP_MAX="$2"
            shift 2
            ;;
        --size-min)
            SIZE_MIN="$2"
            shift 2
            ;;
        --size-max)
            SIZE_MAX="$2"
            shift 2
            ;;
        --barrier-min)
            BARRIER_MIN="$2"
            shift 2
            ;;
        --barrier-max)
            BARRIER_MAX="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --samples N       Number of Monte Carlo samples (default: $SAMPLES)"
            echo "  --threads N       Number of threads to use (default: $THREADS)"
            echo "  --temp-min X      Minimum temperature in K (default: $TEMP_MIN)"
            echo "  --temp-max X      Maximum temperature in K (default: $TEMP_MAX)"
            echo "  --size-min X      Minimum feature size in nm (default: $SIZE_MIN)"
            echo "  --size-max X      Maximum feature size in nm (default: $SIZE_MAX)"
            echo "  --barrier-min X   Minimum barrier height in eV (default: $BARRIER_MIN)"
            echo "  --barrier-max X   Maximum barrier height in eV (default: $BARRIER_MAX)"
            echo "  --help            Display this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Run the test with the configured parameters
echo -e "${YELLOW}Running the quantum wave Monte Carlo test...${NC}"
echo -e "${GREEN}----------------------------------------------${NC}"
./quantum_wave_mc_test \
    --samples $SAMPLES \
    --threads $THREADS \
    --temp-min $TEMP_MIN \
    --temp-max $TEMP_MAX \
    --size-min $SIZE_MIN \
    --size-max $SIZE_MAX \
    --barrier-min $BARRIER_MIN \
    --barrier-max $BARRIER_MAX

# Return to the original directory
cd ..

echo -e "${GREEN}Test completed!${NC}"
