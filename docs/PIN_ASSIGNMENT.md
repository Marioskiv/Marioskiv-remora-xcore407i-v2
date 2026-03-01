# XCore407I Pin Assignment Guide

This guide summarizes practical pin usage for Remora on XCore407I and excludes conflicting or deprecated mappings.

## 1) Reserved Pins (Do Not Use for Motion I/O)

### Ethernet RMII (DP83848I)
- PA1  (ETH_REF_CLK)
- PA2  (ETH_MDIO)
- PA7  (ETH_CRS_DV)
- PC1  (ETH_MDC)
- PC4  (ETH_RXD0)
- PC5  (ETH_RXD1)
- PG11 (ETH_TX_EN)
- PG13 (ETH_TXD0)
- PG14 (ETH_TXD1)

### System and Power
- BOOT0
- RESET
- VBAT
- VREF+
- All 3.3V, 5V, and GND rails

## 2) Standard Motion Mapping (8-Axis Capable)

### Step/Dir/Enable
| Axis | Enable | Step  | Dir   |
|------|--------|-------|-------|
| 0 (X) | PE_3  | PE_4  | PE_5  |
| 1 (Y) | PE_6  | PE_0  | PE_1  |
| 2 (Z) | PB_9  | PB_4  | PB_5  |
| 3 (A) | PB_8  | PD_12 | PD_13 |
| 4 (B) | PD_9  | PD_14 | PD_15 |
| 5 (C) | PD_10 | PG_2  | PG_3  |
| 6 (U) | PG_4  | PG_5  | PG_6  |
| 7 (V) | PG_7  | PG_8  | PG_9  |

### Encoders
| Axis | A Pin | B Pin | Index |
|------|-------|-------|-------|
| 0 (X) | PD_0  | PD_1  | PD_8  |
| 1 (Y) | PD_2  | PD_3  | -     |
| 2 (Z) | PD_4  | PD_5  | -     |
| 3 (A) | PD_6  | PD_7  | -     |
| 4 (B) | PE_7  | PE_8  | -     |
| 5 (C) | PE_9  | PE_10 | -     |
| 6 (U) | PE_11 | PE_12 | -     |
| 7 (V) | PE_13 | PE_14 | -     |

### Endstops and Alarms
- Endstops: `PA_0..PA_7` (bits 0..7)
- Alarms: `PC_0..PC_7` (bits 8..15)

## 3) Differential Alarm Wiring (A+/A-)

For servo drivers exposing differential alarm outputs:
- Connect all `A+` lines to `+3.3V` (shared).
- Connect each `A-` line to a dedicated alarm GPIO (`PC_0..PC_7`).
- Use pull-ups in configuration (`Pullup: true`).
- Use active-low logic (`Invert: true`) when required by the driver.

## 4) Validation Sources

- `examples/README.md` (authoritative axis mapping used by firmware examples)
- `src/stm32f4xx_hal_msp.c` (active Ethernet pin init)
- `resources/schematic xcore407i/ETHERNET DP83848I schematic.json`
- `resources/schematic xcore407i/xcore407i_pin_headers.json`
