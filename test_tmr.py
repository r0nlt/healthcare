#!/usr/bin/env python3
"""
Simple test for the TMR classes with fallback implementation
"""

import sys
sys.path.append("/Users/rishabnuguru/rad-tolerant-ml")
import rad_ml_minimal

# Initialize the framework
rad_ml_minimal.initialize()
print(f"Using rad_ml minimal fallback implementation")

# Create a TMR-protected integer
counter = rad_ml_minimal.StandardTMR(42)
print(f"Initial value: {counter.value}")

# Create a TMR-protected float
sensor = rad_ml_minimal.EnhancedTMR(98.6)
print(f"Initial sensor reading: {sensor.value}")

# Simulate corruption (direct assignment to one of the internal values)
# This is just a demonstration, in real usage the corruption would come from radiation effects
if hasattr(counter, "_tmr") and counter._tmr is not None:
    # If we have the C++ implementation
    print("Using C++ TMR implementation")
    # Counter._tmr would have direct access methods in the C++ implementation
else:
    # Using Python fallback
    print("Using Python fallback implementation")
    # In the fallback, we can directly modify the protected value
    counter._value = 43

# Check integrity
if not counter.check_integrity():
    print("Integrity check failed, attempting correction...")
    corrected = counter.correct()
    if corrected:
        print(f"Successfully corrected value to: {counter.value}")
    else:
        print("Failed to correct value")
else:
    print("No corruption detected")

# Change the value
counter.value = 100
print(f"New value: {counter.value}")

# Shutdown the framework
rad_ml_minimal.shutdown()
print("Framework shutdown successfully") 