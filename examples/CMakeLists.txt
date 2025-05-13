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

# These will be enabled as they are implemented
# add_subdirectory(fault_injection)
# add_subdirectory(radiation_sim) 