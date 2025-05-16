# Healthcare test directory

# Quantum validation test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/quantum_validation_test.cpp")
    add_executable(quantum_validation_test
        quantum_validation_test.cpp
    )

    target_link_libraries(quantum_validation_test
        rad_tolerant_ml_healthcare
    )

    add_test(NAME quantum_validation_test
             COMMAND quantum_validation_test)
endif()

# DNA damage test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dna_damage_test.cpp")
    add_executable(dna_damage_test
        dna_damage_test.cpp
    )

    target_link_libraries(dna_damage_test
        rad_tolerant_ml_healthcare
    )

    add_test(NAME dna_damage_test
             COMMAND dna_damage_test)
endif()

# Cell cycle test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cell_cycle_test.cpp")
    add_executable(cell_cycle_test
        cell_cycle_test.cpp
    )

    target_link_libraries(cell_cycle_test
        rad_tolerant_ml_healthcare
    )

    add_test(NAME cell_cycle_test
             COMMAND cell_cycle_test)
endif()

# Monte Carlo simulation test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/monte_carlo_test.cpp")
    add_executable(monte_carlo_test
        monte_carlo_test.cpp
    )

    target_link_libraries(monte_carlo_test
        rad_tolerant_ml_healthcare
    )

    add_test(NAME monte_carlo_test
             COMMAND monte_carlo_test)
endif()

# Full framework test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/full_framework_test.cpp")
    add_executable(full_framework_test
        full_framework_test.cpp
    )

    target_link_libraries(full_framework_test
        rad_tolerant_ml_healthcare
    )

    add_test(NAME full_framework_test
             COMMAND full_framework_test)
endif()

# Create a test that runs all healthcare tests
add_custom_target(run_healthcare_tests
    DEPENDS
    COMMENT "Running all healthcare tests"
)

# Add existing tests to the custom target
if(TARGET quantum_validation_test)
    add_dependencies(run_healthcare_tests quantum_validation_test)
endif()

if(TARGET dna_damage_test)
    add_dependencies(run_healthcare_tests dna_damage_test)
endif()

if(TARGET cell_cycle_test)
    add_dependencies(run_healthcare_tests cell_cycle_test)
endif()

if(TARGET monte_carlo_test)
    add_dependencies(run_healthcare_tests monte_carlo_test)
endif()

if(TARGET full_framework_test)
    add_dependencies(run_healthcare_tests full_framework_test)
endif()
