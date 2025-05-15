#!/bin/bash

echo "Building and running the Healthcare Quantum Integration module..."

# Create obj directory if it doesn't exist
mkdir -p obj

# Compile the bio_quantum_integration.cpp file
echo "Compiling bio_quantum_integration.cpp..."
g++ -std=c++14 -I./include -c src/healthcare/bio_quantum_integration.cpp -o obj/bio_quantum_integration.o

# Compile the test file
echo "Compiling healthcare_test.cpp..."
g++ -std=c++14 -I./include -c test/healthcare_test.cpp -o obj/healthcare_test.o

# Link the object files to create the executable
echo "Linking..."
g++ obj/bio_quantum_integration.o obj/healthcare_test.o -o healthcare_test

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo -e "\nCompilation successful! Running the test...\n"
    # Run the test
    ./healthcare_test
else
    echo "Compilation failed. Please check the error messages above."
fi
