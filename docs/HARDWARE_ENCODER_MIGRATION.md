# HardwareEncoder Migration Guide

This guide helps you migrate from software Encoder modules to the new STM32 timer-based HardwareEncoder modules.

## Why Migrate

HardwareEncoder uses native STM32 timer encoder mode (quadrature X4), which provides:

- Better high-speed pulse capture reliability
- Lower CPU load in the Base thread
- Built-in counter hardware for precise position tracking
- Cleaner integration with per-axis driver fault feedback

## Migration Scope

You only need to update module entries in your JSON configuration.

- Keep Stepgen modules unchanged
- Replace Encoder modules with HardwareEncoder
- Optionally remove separate Alarm modules if you use integrated fault handling inside HardwareEncoder

## Old vs New JSON

Old software decoder:

```json
{
  "Thread": "Base",
  "Type": "Encoder",
  "Joint Number": 0,
  "Channel A Pin": "PD_0",
  "Channel B Pin": "PD_1",
  "Index Pin": "PD_8"
}
```

New hardware timer decoder:

```json
{
  "Thread": "Base",
  "Type": "HardwareEncoder",
  "Joint Number": 0,
  "Timer": "TIM1",
  "Channel A Pin": "PE_9",
  "Channel B Pin": "PE_11",
  "Fault Pin": "PC_0",
  "Fault Bit": 8,
  "Fault Invert": true,
  "Fault Pullup": true,
  "Fault Latch": true,
  "Fault Clear On Joint Disable": true,
  "Fault Debounce Samples": 3
}
```

## Supported Timer Names

- TIM1
- TIM2
- TIM3
- TIM4
- TIM5
- TIM8
- TIM9
- TIM12

Notes:

- TIM2 and TIM3 are used by Remora Servo/Base thread timers in this firmware variant.
- Using TIM2/TIM3 in HardwareEncoder is blocked by default unless you explicitly set:

```json
"Allow Timer Conflict": true
```

Use this override only if you fully understand the real-time side effects.

## Pin Rules

HardwareEncoder validates pin mapping at startup.

- Channel A and Channel B must belong to the same timer
- They must be timer channel 1 and channel 2 (either order)
- The optional Timer field must match the timer implied by the selected pins

## Overflow and Position Continuity

- 16-bit timer counters are handled with signed delta accumulation so position remains continuous across wrap-around
- 32-bit timers (TIM2/TIM5) also use signed delta accumulation
- Feedback is continuously written to txData joint feedback fields for LinuxCNC transport

## Fault Hardening Options

If Fault Pin and Fault Bit are set, the module can harden fault reporting:

- Fault Invert: active-low or active-high fault polarity
- Fault Pullup: enables internal pull-up on fault input pin
- Fault Debounce Samples: number of stable samples required before state change
- Fault Latch: once fault is detected, keep it asserted until cleared
- Fault Clear On Joint Disable: clear latch when the joint is disabled

Recommended defaults:

- Fault Latch: true
- Fault Debounce Samples: 3
- Fault Clear On Joint Disable: true

## LinuxCNC Side

Use the matching 6-axis HAL template for hardware encoder + fault mapping:

- linuxcnc/xcore407i_6axis_hardware_encoder.hal

This template maps fault bits 8..13 to a combined estop-fault chain.

## Recommended Validation Workflow

1. Flash firmware with updated config JSON
2. Start LinuxCNC with the hardware encoder HAL template
3. Jog each axis and confirm position feedback increments smoothly
4. Trigger one fault input and verify immediate estop path reaction
5. Disable and re-enable joint to validate fault latch clear behavior

## Reference Example

Use this ready configuration as baseline:

- examples/config_6axis_hardware_encoder.json
