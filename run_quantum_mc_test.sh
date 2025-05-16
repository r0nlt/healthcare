#!/bin/bash

# Build and run the quantum wave equation Monte Carlo test
# This script demonstrates how to build and run the test with various parameters

# Colors for better readability
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Step 1: Create build directory if it doesn't exist
echo -e "${BLUE}Creating build directory if it doesn't exist...${NC}"
mkdir -p build
cd build

# Step 2: Run CMake to configure the project
echo -e "${BLUE}Configuring project with CMake...${NC}"
cmake ..

# Step 3: Build the test
echo -e "${BLUE}Building the quantum_wave_mc_test...${NC}"
cmake --build . --target src/test/quantum_wave_mc_test

# Check if build succeeded
if [ $? -ne 0 ]; then
    echo -e "${YELLOW}Initial build target failed, trying alternative build path...${NC}"
    # Try alternative target name without the path
    cmake --build . --target quantum_wave_mc_test

    if [ $? -ne 0 ]; then
        echo -e "${YELLOW}Alternative build target failed, trying to build all targets...${NC}"
        # Try building all targets
        cmake --build .

        if [ $? -ne 0 ]; then
            echo -e "${YELLOW}Build failed. Checking if executable exists anyway...${NC}"
        fi
    fi
fi

# Search for the executable in possible locations
TEST_EXECUTABLE=""
POSSIBLE_PATHS=(
    "./src/test/quantum_wave_mc_test"
    "./quantum_wave_mc_test"
    "./bin/quantum_wave_mc_test"
    "./test/quantum_wave_mc_test"
)

for path in "${POSSIBLE_PATHS[@]}"; do
    if [ -x "$path" ]; then
        TEST_EXECUTABLE="$path"
        break
    fi
done

# If not found, try to find it using the find command
if [ -z "$TEST_EXECUTABLE" ]; then
    echo -e "${YELLOW}Searching for quantum_wave_mc_test executable...${NC}"
    TEST_EXECUTABLE=$(find . -name "quantum_wave_mc_test" -type f -perm -u+x | head -n 1)
fi

if [ -z "$TEST_EXECUTABLE" ]; then
    echo -e "${YELLOW}ERROR: Could not find the quantum_wave_mc_test executable. Build may have failed.${NC}"
    echo -e "${YELLOW}List of files in current directory:${NC}"
    ls -la
    echo -e "${YELLOW}List of files in src/test directory (if it exists):${NC}"
    if [ -d "src/test" ]; then
        ls -la src/test
    else
        echo "src/test directory not found"
    fi
    cd ..
    exit 1
fi

echo -e "${GREEN}Found executable at: ${TEST_EXECUTABLE}${NC}"

# Step 4: Run the test with specified parameters
echo -e "${YELLOW}Running the quantum wave Monte Carlo test...${NC}"
echo -e "${GREEN}----------------------------------------------${NC}"

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
"$TEST_EXECUTABLE" \
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
