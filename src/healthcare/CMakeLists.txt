cmake_minimum_required(VERSION 3.14)

# Healthcare module components
set(HEALTHCARE_SOURCES
    bio_quantum_integration.cpp
    radiation_therapy_model.cpp
    chemotherapy/chemo_quantum_model.cpp
    chemotherapy/chemoradiation_synergy.cpp
    chemotherapy/drug_target_interaction.cpp
)

# Create library
add_library(rad_ml_healthcare ${HEALTHCARE_SOURCES})

# Include directories
target_include_directories(rad_ml_healthcare PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Build the QM/MM integration test executable
add_executable(qmmm_integration_test
    qmmm_integration_test.cpp
)

# Link against the healthcare library
target_link_libraries(qmmm_integration_test
    rad_ml_healthcare
)

# Set C++14 standard for the test executable
set_target_properties(qmmm_integration_test PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Install targets
install(TARGETS
    rad_ml_healthcare
    qmmm_integration_test
    DESTINATION bin
)
