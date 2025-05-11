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

# These will be enabled as they are implemented
# add_subdirectory(fault_injection)
# add_subdirectory(radiation_sim) 