add_executable(mission_simulator mission_simulator.cpp)
target_link_libraries(mission_simulator PRIVATE 
    rad_ml_research 
    rad_ml_testing 
    rad_ml_tmr 
    rad_ml_qft
) 