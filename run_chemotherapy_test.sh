#!/bin/bash

# Run chemotherapy module test script
# For Radiation-Based Healthcare Quantum Modeling Framework
# Copyright (c) 2025 Rishab Nuguru

# Color output helpers
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=========================================================${NC}"
echo -e "${BLUE}Radiation-Based Healthcare Quantum Modeling Framework${NC}"
echo -e "${BLUE}Chemotherapy Module Test${NC}"
echo -e "${BLUE}=========================================================${NC}"

# Check if CMake is installed
if ! command -v cmake &> /dev/null
then
    echo -e "${RED}CMake not found. Please install CMake first.${NC}"
    exit 1
fi

# Configure the project with CMake
echo -e "${YELLOW}Configuring with CMake...${NC}"
cmake . -DCMAKE_BUILD_TYPE=Release

# Build the chemotherapy test
echo -e "${YELLOW}Building the chemotherapy test...${NC}"
cmake --build . --target chemotherapy_test -- -j4

# Check if the build was successful
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed. Please check the errors above.${NC}"
    exit 1
fi

# Run the test
echo -e "${YELLOW}Running the chemotherapy test...${NC}"
echo -e "${BLUE}=========================================================${NC}"
./chemotherapy_test
TEST_EXIT_CODE=$?
echo -e "${BLUE}=========================================================${NC}"

# Check if the test was successful
if [ $TEST_EXIT_CODE -eq 0 ]; then
    echo -e "${GREEN}Chemotherapy test completed successfully!${NC}"
else
    echo -e "${RED}Chemotherapy test failed with exit code ${TEST_EXIT_CODE}.${NC}"
fi

exit $TEST_EXIT_CODE
