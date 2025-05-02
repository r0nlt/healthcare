add_executable(tmr_test tmr_test.cpp)
target_link_libraries(tmr_test PRIVATE rad_ml)

add_test(NAME TMRTest COMMAND tmr_test) 