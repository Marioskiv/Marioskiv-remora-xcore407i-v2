#include "pinmap.h"
#include <stddef.h>

// Find peripheral instance from pin map table
uint32_t pinmap_peripheral(PinName pin, const PinMap* map)
{
    if (pin == NC || map == NULL) {
        return (uint32_t)NC;
    }
    
    // Search through pin map for matching pin
    while (map->pin != NC) {
        if (map->pin == pin) {
            return map->peripheral;
        }
        map++;
    }
    
    // Not found
    return (uint32_t)NC;
}

// Find function code from pin map table
uint32_t pinmap_function(PinName pin, const PinMap* map)
{
    if (pin == NC || map == NULL) {
        return (uint32_t)NC;
    }
    
    // Search through pin map for matching pin
    while (map->pin != NC) {
        if (map->pin == pin) {
            return map->function;
        }
        map++;
    }
    
    // Not found
    return (uint32_t)NC;
}

// Merge two peripheral selections
// Both must be the same peripheral, or one must be NC
uint32_t pinmap_merge(uint32_t per1, uint32_t per2)
{
    // If either is NC, return the other
    if (per1 == (uint32_t)NC) {
        return per2;
    }
    if (per2 == (uint32_t)NC) {
        return per1;
    }
    
    // Both must match
    if (per1 == per2) {
        return per1;
    }
    
    // Mismatch - invalid configuration
    return (uint32_t)NC;
}
