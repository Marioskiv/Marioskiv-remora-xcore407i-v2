#ifndef MBED_PINMAP_H
#define MBED_PINMAP_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Pin name type (STM32 port+pin encoding)
typedef uint32_t PinName;

// Pin map structure
typedef struct {
    PinName pin;
    uint32_t peripheral;
    uint32_t function;
} PinMap;

// Not connected marker
#define NC (PinName)0xFFFFFFFF

// Find peripheral from pin map
uint32_t pinmap_peripheral(PinName pin, const PinMap* map);

// Find function code from pin map
uint32_t pinmap_function(PinName pin, const PinMap* map);

// Merge two peripheral selections (must match or one NC)
uint32_t pinmap_merge(uint32_t per1, uint32_t per2);

#ifdef __cplusplus
}
#endif

#endif // MBED_PINMAP_H
