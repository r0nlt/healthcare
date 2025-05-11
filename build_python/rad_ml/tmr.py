"""
Triple Modular Redundancy (TMR) wrapper classes

This module provides more Pythonic wrappers around the C++ TMR classes.
"""

try:
    # Try to import the core bindings
    from ._core import (
        StandardTMRInt, StandardTMRFloat, StandardTMRDouble,
        create_standard_tmr_int, create_standard_tmr_float, create_standard_tmr_double,
        create_enhanced_tmr_int, create_enhanced_tmr_float, create_enhanced_tmr_double
    )
except ImportError as e:
    print(f"Warning: Could not import some C++ bindings: {e}")
    # Define fallback versions if needed

class StandardTMR:
    """Python wrapper for StandardTMR classes to handle different numeric types"""
    
    def __init__(self, initial_value=0):
        """Initialize StandardTMR with the appropriate type based on the initial value"""
        self._value = initial_value
        try:
            if isinstance(initial_value, int):
                self._tmr = create_standard_tmr_int(initial_value)
            elif isinstance(initial_value, float):
                self._tmr = create_standard_tmr_float(initial_value)
            else:
                raise TypeError(f"Unsupported type: {type(initial_value)}. Only int and float are supported.")
        except Exception as e:
            print(f"Warning: Failed to create TMR: {e}")
            # Fallback to a basic implementation
            self._tmr = None
    
    @property
    def value(self):
        """Get the protected value"""
        if self._tmr is None:
            return self._value
        return self._tmr.get_value()
    
    @value.setter
    def value(self, new_value):
        """Set the protected value"""
        if not isinstance(new_value, type(self._value)):
            raise TypeError(f"Expected {type(self._value)}, got {type(new_value)}")
        self._value = new_value
        if self._tmr is not None:
            self._tmr.set_value(new_value)
    
    def correct(self):
        """Attempt to correct errors in the protected value"""
        if self._tmr is None:
            return False
        return self._tmr.correct()
    
    def check_integrity(self):
        """Check if the protected value has integrity issues"""
        if self._tmr is None:
            return True
        return self._tmr.check_integrity()
    
    def __str__(self):
        return f"StandardTMR({self.value})"
    
    def __repr__(self):
        return f"StandardTMR({self.value})"


class EnhancedTMR:
    """Python wrapper for EnhancedTMR classes to handle different numeric types"""
    
    def __init__(self, initial_value=0):
        """Initialize EnhancedTMR with the appropriate type based on the initial value"""
        self._value = initial_value
        try:
            if isinstance(initial_value, int):
                self._tmr = create_enhanced_tmr_int(initial_value)
            elif isinstance(initial_value, float):
                self._tmr = create_enhanced_tmr_float(initial_value)
            else:
                raise TypeError(f"Unsupported type: {type(initial_value)}. Only int and float are supported.")
        except Exception as e:
            print(f"Warning: Failed to create EnhancedTMR: {e}")
            # Fallback to a basic implementation
            self._tmr = None
    
    @property
    def value(self):
        """Get the protected value"""
        if self._tmr is None:
            return self._value
        return self._tmr.get_value()
    
    @value.setter
    def value(self, new_value):
        """Set the protected value"""
        if not isinstance(new_value, type(self._value)):
            raise TypeError(f"Expected {type(self._value)}, got {type(new_value)}")
        self._value = new_value
        if self._tmr is not None:
            self._tmr.set_value(new_value)
    
    def correct(self):
        """Attempt to correct errors in the protected value"""
        if self._tmr is None:
            return False
        return self._tmr.correct()
    
    def check_integrity(self):
        """Check if the protected value has integrity issues"""
        if self._tmr is None:
            return True
        return self._tmr.check_integrity()
    
    def __str__(self):
        return f"EnhancedTMR({self.value})"
    
    def __repr__(self):
        return f"EnhancedTMR({self.value})" 