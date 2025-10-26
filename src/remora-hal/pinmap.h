/* mbed Microcontroller Library
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************
 * Minimal pinmap header for STM32F407IGT6
 */

#ifndef MBED_PINMAP_H
#define MBED_PINMAP_H

#include "PinNames.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// PinMap struct is defined in PinNames.h

// Function prototypes
uint32_t pinmap_peripheral(PinName pin, const PinMap *map);
uint32_t pinmap_function(PinName pin, const PinMap *map);
uint32_t pinmap_merge(uint32_t a, uint32_t b);
void pinmap_pinout(PinName pin, const PinMap *map);
uint32_t pinmap_find_peripheral(PinName pin, const PinMap *map);

#ifdef __cplusplus
}
#endif

#endif
