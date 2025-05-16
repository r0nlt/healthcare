cmake_minimum_required(VERSION 3.10)
project(rad_tolerant_ml_healthcare VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Find required packages
find_package(Eigen3 QUIET)
if(NOT EIGEN3_FOUND)
    # If not found, use a fallback approach or display a message
    message(STATUS "Eigen3 not found - using bundled version or system headers")
endif()

# Source files
set(PHYSICS_SOURCES
    src/physics/quantum_integration.cpp
    # Comment out this file to avoid duplicate symbol errors
    # src/rad_ml/physics/quantum_models.cpp
)

# Add a separate variable for quantum_models when needed
set(QUANTUM_MODELS_SOURCES
    src/rad_ml/physics/quantum_models.cpp
)

set(HEALTHCARE_SOURCES
    src/healthcare/bio_quantum_integration.cpp
)

set(NEURAL_SOURCES
    # The example now includes all the necessary implementation
    # src/rad_ml/neural/qft_enhanced_network_example.cpp
)

# Create healthcare test executable
add_executable(healthcare_test
    test/healthcare_test.cpp
    ${HEALTHCARE_SOURCES}
)

# Create QFT bridge test executable
add_executable(qft_bridge_test
    test/qft_bridge_test.cpp
    src/rad_ml/crossdomain/qft_bridge.cpp
    ${PHYSICS_SOURCES}
    ${QUANTUM_MODELS_SOURCES}
    ${HEALTHCARE_SOURCES}
)

# Create simplified cross-domain bridge example
add_executable(crossdomain_simplified
    examples/crossdomain_simplified.cpp
)

# Create QFT bridge analysis program
add_executable(qft_bridge_analysis
    examples/qft_bridge_analysis.cpp
)

# Create QFT-enhanced neural network example
add_executable(qft_enhanced_network_example
    examples/qft_enhanced_network_example.cpp
    ${NEURAL_SOURCES}
    ${PHYSICS_SOURCES}
    ${QUANTUM_MODELS_SOURCES}
    ${HEALTHCARE_SOURCES}
    src/rad_ml/crossdomain/qft_bridge.cpp
)

# Set output directory for executables
set_target_properties(
    healthcare_test
    qft_bridge_test
    crossdomain_simplified
    qft_bridge_analysis
    qft_enhanced_network_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Print configuration summary
message(STATUS "Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Include directory: ${CMAKE_CURRENT_SOURCE_DIR}/include")

# Add healthcare and chemotherapy modules
add_library(rad_ml_healthcare
    src/healthcare/bio_quantum_integration.cpp
    src/healthcare/radiation_therapy_model.cpp
    src/healthcare/chemotherapy/chemo_quantum_model.cpp
    src/healthcare/chemotherapy/drug_target_interaction.cpp
    src/healthcare/chemotherapy/chemoradiation_synergy.cpp
)

# Build the chemotherapy test
add_executable(chemotherapy_test
    src/healthcare/chemotherapy/chemotherapy_test.cpp
)
target_link_libraries(chemotherapy_test rad_ml_healthcare)

# Add install targets for healthcare library
install(TARGETS rad_ml_healthcare DESTINATION lib)
install(DIRECTORY include/rad_ml/healthcare DESTINATION include/rad_ml)
