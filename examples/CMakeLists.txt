add_subdirectory(simple_nn)
add_subdirectory(mission_simulator)

# Add the architecture testing example
add_executable(architecture_test architecture_test.cpp)
target_link_libraries(architecture_test
    rad_ml_research
    rad_ml_tmr
    rad_ml_testing
    rad_ml_qft
)

# Add the residual network example
add_executable(residual_network_test residual_network_test.cpp)
target_link_libraries(residual_network_test
    rad_ml_research
    rad_ml_tmr
    rad_ml_testing
    rad_ml_qft
)

# Add auto_arch_search_example
add_executable(auto_arch_search_example auto_arch_search_example.cpp)
target_link_libraries(auto_arch_search_example rad_ml_research rad_ml_tmr)

# Add radiation_aware_training_example
add_executable(radiation_aware_training_example radiation_aware_training_example.cpp)
target_link_libraries(radiation_aware_training_example rad_ml_research)

# Add tests
add_test(NAME architecture_test_run COMMAND architecture_test)
add_test(NAME residual_network_test_run COMMAND residual_network_test)
add_test(NAME auto_arch_search_example_run COMMAND auto_arch_search_example)
add_test(NAME radiation_aware_training_example_run COMMAND radiation_aware_training_example)

# Installation
install(TARGETS architecture_test residual_network_test auto_arch_search_example radiation_aware_training_example
    DESTINATION bin/examples)

# These will be enabled as they are implemented
# add_subdirectory(fault_injection)
# add_subdirectory(radiation_sim)

# Healthcare examples - only built if BUILD_HEALTHCARE is enabled
if(BUILD_HEALTHCARE)
    # Include directories
    include_directories(${PROJECT_SOURCE_DIR}/include)

    # Enhanced healthcare example
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/enhanced_healthcare_example.cpp")
        add_executable(enhanced_healthcare_example
            enhanced_healthcare_example.cpp
        )

        target_link_libraries(enhanced_healthcare_example
            rad_tolerant_ml_healthcare
        )

        set_target_properties(enhanced_healthcare_example
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

        install(TARGETS enhanced_healthcare_example
            DESTINATION bin/examples)
    endif()

    # Cell cycle effects example
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cell_cycle_effects.cpp")
        add_executable(cell_cycle_effects
            cell_cycle_effects.cpp
        )

        target_link_libraries(cell_cycle_effects
            rad_tolerant_ml_healthcare
        )

        set_target_properties(cell_cycle_effects
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()

    # Monte Carlo simulation example
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/monte_carlo_simulation.cpp")
        add_executable(monte_carlo_simulation
            monte_carlo_simulation.cpp
        )

        target_link_libraries(monte_carlo_simulation
            rad_tolerant_ml_healthcare
        )

        set_target_properties(monte_carlo_simulation
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()

    # Parameter optimization example
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/parameter_optimization.cpp")
        add_executable(parameter_optimization
            parameter_optimization.cpp
        )

        target_link_libraries(parameter_optimization
            rad_tolerant_ml_healthcare
        )

        set_target_properties(parameter_optimization
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()

    # DICOM integration example
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dicom_integration.cpp")
        add_executable(dicom_integration
            dicom_integration.cpp
        )

        target_link_libraries(dicom_integration
            rad_tolerant_ml_healthcare
        )

        set_target_properties(dicom_integration
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()
endif()
