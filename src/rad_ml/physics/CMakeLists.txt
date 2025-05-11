# File: src/rad_ml/physics/CMakeLists.txt

# Set sources for physics components
set(RAD_ML_PHYSICS_SOURCES
    quantum_field_theory.cpp
    quantum_models.cpp
)

# Create physics library
add_library(rad_ml_physics ${RAD_ML_PHYSICS_SOURCES})

# Set include directories
target_include_directories(rad_ml_physics
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(rad_ml_physics
    PUBLIC
        rad_ml_core
)

# Export library
install(TARGETS rad_ml_physics
    EXPORT RadMLTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
) 