#!/usr/bin/env python3
"""
Advanced TMR Demo for rad_ml - This demonstrates how TMR protects against radiation-induced errors

This script creates a simplified but comprehensive validation of the Triple Modular Redundancy
(TMR) functionality in the rad_ml framework, showing how it can protect data from radiation effects.
"""

import sys
import random
import time

sys.path.append("/Users/rishabnuguru/rad-tolerant-ml")
import rad_ml_minimal

class EnhancedTMRDemo:
    """A demonstration version of TMR with exposed internal state for testing purposes"""
    
    def __init__(self, initial_value=0):
        """Create a TMR-protected value with 3 copies"""
        self.value_type = type(initial_value)
        # Create three copies of the value (simulating TMR hardware)
        self.value1 = initial_value
        self.value2 = initial_value
        self.value3 = initial_value
    
    @property
    def value(self):
        """Get the majority-voted value"""
        # Simple majority voting
        if self.value1 == self.value2:
            return self.value1
        if self.value1 == self.value3:
            return self.value1
        if self.value2 == self.value3:
            return self.value2
        # If all values are different, use value1 by default
        return self.value1
    
    @value.setter
    def value(self, new_value):
        """Set all three copies to the new value"""
        if not isinstance(new_value, self.value_type):
            raise TypeError(f"Expected {self.value_type}, got {type(new_value)}")
        self.value1 = new_value
        self.value2 = new_value
        self.value3 = new_value
    
    def check_integrity(self):
        """Check if all three copies are identical"""
        return self.value1 == self.value2 == self.value3
    
    def correct(self):
        """Attempt to correct errors using majority voting"""
        if self.value1 == self.value2 and self.value1 != self.value3:
            # Copy 3 is corrupted, fix it
            self.value3 = self.value1
            return True
        elif self.value1 == self.value3 and self.value1 != self.value2:
            # Copy 2 is corrupted, fix it
            self.value2 = self.value1
            return True
        elif self.value2 == self.value3 and self.value1 != self.value2:
            # Copy 1 is corrupted, fix it
            self.value1 = self.value2
            return True
        # If all values are different or all are the same, can't correct
        return False
    
    def __str__(self):
        return f"TMR({self.value}) [copies: {self.value1}, {self.value2}, {self.value3}]"


def simulate_bit_flip(value, bit_position):
    """Simulate a radiation-induced bit flip at a specific position"""
    # Convert to binary representation
    if isinstance(value, int):
        binary = list(bin(value)[2:].zfill(32))
        # Only flip if within range of the binary representation
        if bit_position < len(binary):
            # Flip the specified bit
            binary[bit_position] = '1' if binary[bit_position] == '0' else '0'
            # Convert back to integer
            return int(''.join(binary), 2)
        return value  # No change if bit position is out of range
    elif isinstance(value, float):
        # For floats, we convert to an integer representation, flip a bit, then convert back
        import struct
        # Convert float to its IEEE 754 binary representation
        ieee = struct.pack('>f', value)
        # Convert to integer
        i = struct.unpack('>I', ieee)[0]
        # Flip a bit
        i ^= (1 << bit_position)
        # Convert back to float
        return struct.unpack('>f', struct.pack('>I', i))[0]
    else:
        raise TypeError(f"Unsupported type: {type(value)}")


def run_tmr_demonstration(num_tests=10):
    """Run a demonstration of TMR protection"""
    
    print("\n==== TMR Protection Demonstration ====")
    print("This demonstrates how TMR protects against radiation-induced bit flips")
    print("by maintaining three copies of data and using majority voting.\n")
    
    # Initialize counters
    successful_corrections = 0
    failed_corrections = 0
    no_corruption_needed = 0
    
    # Initialize the framework
    rad_ml_minimal.initialize()
    print("Radiation-tolerant framework initialized")
    
    for i in range(num_tests):
        # Create a value, alternating between int and float
        if i % 2 == 0:
            original_value = random.randint(1, 1000)
            value_type = "integer"
        else:
            original_value = round(random.uniform(1.0, 1000.0), 2)
            value_type = "float"
        
        # Create our TMR-protected value
        tmr = EnhancedTMRDemo(original_value)
        
        print(f"\nTest {i+1}: Created TMR-protected {value_type} = {original_value}")
        print(f"  Initial state: {tmr}")
        
        # Simulate radiation effect
        should_corrupt = random.choice([True, True, False])  # 66% chance of corruption
        
        if should_corrupt:
            # Choose which copy to corrupt and which bit to flip
            corrupt_copy = random.randint(1, 3)
            bit_to_flip = random.randint(0, 7)  # Target lower bits for more visible effects
            
            # Apply the bit flip to the chosen copy
            if corrupt_copy == 1:
                corrupted_value = simulate_bit_flip(tmr.value1, bit_to_flip)
                tmr.value1 = corrupted_value
                corrupted_copy = "value1"
            elif corrupt_copy == 2:
                corrupted_value = simulate_bit_flip(tmr.value2, bit_to_flip)
                tmr.value2 = corrupted_value
                corrupted_copy = "value2"
            else:
                corrupted_value = simulate_bit_flip(tmr.value3, bit_to_flip)
                tmr.value3 = corrupted_value
                corrupted_copy = "value3"
            
            print(f"  Simulating radiation effect: Bit {bit_to_flip} flipped in {corrupted_copy}")
            print(f"  After corruption: {tmr}")
            print(f"  Majority-voted value: {tmr.value}")
            
            # Check integrity
            if not tmr.check_integrity():
                print("  ✓ Integrity check detected the corruption!")
                
                # Attempt to correct
                if tmr.correct():
                    print("  ✓ Error successfully corrected by TMR majority voting")
                    print(f"  After correction: {tmr}")
                    
                    # Verify the correction recovered the original value
                    if tmr.value == original_value:
                        print(f"  ✓ Original value {original_value} correctly preserved")
                        successful_corrections += 1
                    else:
                        print(f"  ✗ Value incorrectly changed to {tmr.value}")
                        failed_corrections += 1
                else:
                    print("  ✗ Too many copies corrupted, correction failed")
                    failed_corrections += 1
            else:
                print("  ✓ Integrity check passed (multiple copies had same corruption or no effect)")
                if tmr.value == original_value:
                    print(f"  ✓ No value change occurred")
                    no_corruption_needed += 1
                else:
                    print(f"  ✗ Value changed from {original_value} to {tmr.value} but not detected")
                    failed_corrections += 1
        else:
            print("  No radiation effect simulated")
            
            # Check integrity - should confirm no corruption
            if tmr.check_integrity():
                print("  ✓ Integrity verified (no corruption)")
                no_corruption_needed += 1
            else:
                print("  ✗ False integrity failure (should not happen)")
                failed_corrections += 1
    
    # Shutdown the framework
    rad_ml_minimal.shutdown()
    print("\nFramework shutdown successfully")
    
    # Report results
    print("\n==== TMR Protection Results ====")
    print(f"Total tests: {num_tests}")
    print(f"Successful corrections: {successful_corrections} ({successful_corrections/num_tests*100:.1f}%)")
    print(f"Failed corrections: {failed_corrections} ({failed_corrections/num_tests*100:.1f}%)")
    print(f"No correction needed: {no_corruption_needed} ({no_corruption_needed/num_tests*100:.1f}%)")
    print(f"Protection rate: {(successful_corrections + no_corruption_needed)/num_tests*100:.1f}%")
    
    return successful_corrections, failed_corrections, no_corruption_needed


if __name__ == "__main__":
    run_tmr_demonstration(10) 