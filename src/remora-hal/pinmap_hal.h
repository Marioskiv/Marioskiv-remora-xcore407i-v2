/* mbed Microcontroller Library
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************
 * Minimal pinmap_hal header for STM32F407IGT6
 */

#ifndef MBED_PINMAP_HAL_H
#define MBED_PINMAP_HAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Pin data structure for ADC/PWM mappings
#define STM_MODE_INPUT              (0)
#define STM_MODE_OUTPUT_PP          (1)
#define STM_MODE_OUTPUT_OD          (2)
#define STM_MODE_AF_PP              (3)
#define STM_MODE_AF_OD              (4)
#define STM_MODE_ANALOG             (5)
#define STM_MODE_IT_RISING          (6)
#define STM_MODE_IT_FALLING         (7)
#define STM_MODE_IT_RISING_FALLING  (8)
#define STM_MODE_EVT_RISING         (9)
#define STM_MODE_EVT_FALLING        (10)
#define STM_MODE_EVT_RISING_FALLING (11)
#define STM_MODE_IT_EVT_RESET       (12)

// Pin modes mask and shift
#define STM_PIN_PUPD_MASK   0x03
#define STM_PIN_PUPD_SHIFT  0
#define STM_PIN_AFNUM_MASK  0x0F
#define STM_PIN_AFNUM_SHIFT 2
#define STM_PIN_OD_MASK     0x01
#define STM_PIN_OD_SHIFT    6
#define STM_PIN_CHANNEL_MASK  0x0F
#define STM_PIN_CHANNEL_SHIFT 7
#define STM_PIN_INVERTED_MASK 0x01
#define STM_PIN_INVERTED_SHIFT 11

#define STM_PIN_DEFINE(FUNC_OD, PUPD, AFNUM)  ((int)(FUNC_OD) | \
                                               (((PUPD) & STM_PIN_PUPD_MASK) << STM_PIN_PUPD_SHIFT) | \
                                               (((AFNUM) & STM_PIN_AFNUM_MASK) << STM_PIN_AFNUM_SHIFT))

#define STM_PIN_DEFINE_EXT(FUNC_OD, PUPD, AFNUM, CHAN, INVERTED)  ((int)(FUNC_OD) | \
                                                                   (((PUPD)  & STM_PIN_PUPD_MASK) << STM_PIN_PUPD_SHIFT) | \
                                                                   (((AFNUM) & STM_PIN_AFNUM_MASK) << STM_PIN_AFNUM_SHIFT) | \
                                                                   (((CHAN)  & STM_PIN_CHANNEL_MASK) << STM_PIN_CHANNEL_SHIFT) | \
                                                                   (((INVERTED) & STM_PIN_INVERTED_MASK) << STM_PIN_INVERTED_SHIFT))

#define STM_PIN_DATA(FUNC_OD, PUPD, AFNUM)     STM_PIN_DEFINE(FUNC_OD, PUPD, AFNUM)
#define STM_PIN_DATA_EXT(FUNC_OD, PUPD, AFNUM, CHAN, INVERTED) STM_PIN_DEFINE_EXT(FUNC_OD, PUPD, AFNUM, CHAN, INVERTED)

#ifdef __cplusplus
}
#endif

#endif
