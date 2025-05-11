add_executable(simple_nn main.cpp)
target_link_libraries(simple_nn PRIVATE 
    rad_ml_research 
    rad_ml_testing 
    rad_ml_tmr
    rad_ml_qft
) 