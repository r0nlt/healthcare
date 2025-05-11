# File: src/rad_ml/sim/CMakeLists.txt

# Make sure environment.cpp is listed in the sources
set(RAD_ML_SIM_SOURCES
    environment.cpp
    # Other simulation source files will be added here
)

# Create simulation library
add_library(rad_ml_sim STATIC ${RAD_ML_SIM_SOURCES})

# Set include directories
target_include_directories(rad_ml_sim
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Link dependencies - don't depend on rad_ml_core as it might not be fully built yet
# and we don't want circular dependencies
target_link_libraries(rad_ml_sim
    PUBLIC
        ${CMAKE_THREAD_LIBS_INIT}
)

# Export library
install(TARGETS rad_ml_sim
    EXPORT RadMLTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
) 