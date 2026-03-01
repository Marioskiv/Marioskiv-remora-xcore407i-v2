# XCore407I Pin Reference (Remora Ethernet Firmware)

This document is the canonical pin map for the XCore407I STM32F407IGT6 build used by this repository.

## Step/Dir/Enable Pins (8-axis capable)

| Axis | Enable | Step  | Direction |
|------|--------|-------|-----------|
| 0 (X) | PE_3  | PE_4  | PE_5      |
| 1 (Y) | PE_6  | PE_0  | PE_1      |
| 2 (Z) | PB_9  | PB_4  | PB_5      |
| 3 (A) | PB_8  | PD_12 | PD_13     |
| 4 (B) | PD_9  | PD_14 | PD_15     |
| 5 (C) | PD_10 | PG_2  | PG_3      |
| 6 (U) | PG_4  | PG_5  | PG_6      |
| 7 (V) | PG_7  | PG_8  | PG_9      |

## Encoder Pins

| Axis | Channel A | Channel B | Index (optional) |
|------|-----------|-----------|------------------|
| 0 (X) | PD_0     | PD_1      | PD_8             |
| 1 (Y) | PD_2     | PD_3      | -                |
| 2 (Z) | PD_4     | PD_5      | -                |
| 3 (A) | PD_6     | PD_7      | -                |
| 4 (B) | PE_7     | PE_8      | -                |
| 5 (C) | PE_9     | PE_10     | -                |
| 6 (U) | PE_11    | PE_12     | -                |
| 7 (V) | PE_13    | PE_14     | -                |

## Endstop Pins (input bits 0-7)

| Axis | Pin  | Input Bit |
|------|------|-----------|
| 0 (X) | PA_0 | 0 |
| 1 (Y) | PA_1 | 1 |
| 2 (Z) | PA_2 | 2 |
| 3 (A) | PA_3 | 3 |
| 4 (B) | PA_4 | 4 |
| 5 (C) | PA_5 | 5 |
| 6 (U) | PA_6 | 6 |
| 7 (V) | PA_7 | 7 |

## Alarm Pins (input bits 8-15)

| Axis | Pin  | Input Bit |
|------|------|-----------|
| 0 (X) | PC_0 | 8  |
| 1 (Y) | PC_1 | 9  |
| 2 (Z) | PC_2 | 10 |
| 3 (A) | PC_3 | 11 |
| 4 (B) | PC_4 | 12 |
| 5 (C) | PC_5 | 13 |
| 6 (U) | PC_6 | 14 |
| 7 (V) | PC_7 | 15 |

## Reserved Ethernet RMII Pins (Do Not Reuse)

These pins are used by the native Ethernet DP83848I interface and must not be used for motion I/O:

- PA1  (ETH_REF_CLK)
- PA2  (ETH_MDIO)
- PA7  (ETH_CRS_DV)
- PC1  (ETH_MDC)
- PC4  (ETH_RXD0)
- PC5  (ETH_RXD1)
- PG11 (ETH_TX_EN)
- PG13 (ETH_TXD0)
- PG14 (ETH_TXD1)

## Other Reserved System Pins

- BOOT0
- RESET
- VBAT
- VREF+
- All 3.3V, 5V, and GND rails

## Reference Sources

- `examples/README.md` (axis pin tables)
- `src/stm32f4xx_hal_msp.c` (active ETH pin initialization)
- `resources/schematic xcore407i/ETHERNET DP83848I schematic.json`
- `resources/schematic xcore407i/xcore407i_pin_headers.json`
