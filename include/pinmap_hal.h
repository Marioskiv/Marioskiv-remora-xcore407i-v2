#ifndef MBED_PINMAP_HAL_H
#define MBED_PINMAP_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

// STM32-specific pin data encoding macros
#define STM_PIN_DATA(mode, pull, afnum) ((int)(((afnum) & 0x0F) << 8) | ((pull) & 0x03) << 4 | ((mode) & 0x0F))
#define STM_PIN_AFNUM(data)              (((data) >> 8) & 0x0F)
#define STM_PIN_MODE(data)               ((data) & 0x0F)
#define STM_PIN_PULL(data)               (((data) >> 4) & 0x03)

// Timer channel encoding (bits 16-19)
#define STM_PIN_CHAN_SHIFT  16
#define STM_PIN_CHAN_MASK   0x0F

// Timer inverted output encoding (bit 20)
#define STM_PIN_INV_SHIFT   20
#define STM_PIN_INV_MASK    0x01

// Extended pin data for PWM (includes channel + AF)
#define STM_PIN_DATA_EXT(mode, pull, afnum, channel, inverted) \
    ((int)(((inverted) & STM_PIN_INV_MASK) << STM_PIN_INV_SHIFT) | \
           (((channel) & STM_PIN_CHAN_MASK) << STM_PIN_CHAN_SHIFT) | \
           (((afnum) & 0x0F) << 8) | \
           (((pull) & 0x03) << 4) | \
           ((mode) & 0x0F))

#ifdef __cplusplus
}
#endif

#endif // MBED_PINMAP_HAL_H
