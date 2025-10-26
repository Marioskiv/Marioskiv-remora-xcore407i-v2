# 📌 XCore407I Pin Reference - 6-Axis Closed-Loop System# PIN REFERENCE - 8-Axis Closed-Loop Configuration



## 📋 Table of Contents## Quick Pin Assignment Table

1. [Quick Reference Tables](#quick-reference-tables)

2. [Reserved Pins (DO NOT USE)](#reserved-pins)| Axis | Step Pin | Dir Pin | Enable Pin | Enc A Pin | Enc B Pin | Endstop Pin | Alarm Pin |

3. [Complete Pin Mapping by Axis](#complete-pin-mapping-by-axis)|------|----------|---------|------------|-----------|-----------|-------------|-----------|

4. [Alarm Wiring (Differential A+/A-)](#alarm-wiring)| **0 (X)** | PE_4 | PE_5 | PE_3 | PD_0 | PD_1 | PA_0 | PC_0 |

5. [Power Pins](#power-pins)| **1 (Y)** | PE_0 | PE_1 | PE_6 | PD_2 | PD_3 | PA_1 | PC_1 |

6. [Available Pins for Expansion](#available-pins)| **2 (Z)** | PB_4 | PB_5 | PB_9 | PD_4 | PD_5 | PA_2 | PC_2 |

| **3 (A)** | PD_12 | PD_13 | PB_8 | PD_6 | PD_7 | PA_3 | PC_3 |

---| **4 (B)** | PD_14 | PD_15 | PD_9 | PE_7 | PE_8 | PA_4 | PC_4 |

| **5 (C)** | PG_2 | PG_3 | PD_10 | PE_9 | PE_10 | PA_5 | PC_5 |

## 🎯 Quick Reference Tables| **6 (U)** | PG_5 | PG_6 | PG_4 | PE_11 | PE_12 | PA_6 | PC_6 |

| **7 (V)** | PG_8 | PG_9 | PG_7 | PE_13 | PE_14 | PA_7 | PC_7 |

### Linear Axes (XYZ)

| Axis | Step | Dir | Enable | Enc A | Enc B | Alarm A- | Endstop | Joint # |**Status LED**: PE_2

|------|------|-----|--------|-------|-------|----------|---------|---------|

| **X** | PE_4 | PE_5 | PE_3 | PD_0 | PD_1 | PC_0 | PA_0 | 0 |---

| **Y** | PE_0 | PE_1 | PE_6 | PD_2 | PD_3 | PC_1 | PA_1 | 1 |

| **Z** | PB_4 | PB_5 | PB_9 | PD_4 | PD_5 | PC_2 | PA_2 | 2 |## Port Groupings



### Rotary Axes (ABC)### GPIOA (Endstops)

| Axis | Step | Dir | Enable | Enc A | Enc B | Alarm A- | Endstop | Joint # |```

|------|------|-----|--------|-------|-------|----------|---------|---------|PA_0 - Axis 0 Endstop

| **A** | PB_6 | PB_7 | PB_8 | PD_6 | PD_7 | PC_3 | PA_3 | 3 |PA_1 - Axis 1 Endstop

| **B** | PC_6 | PC_7 | PC_8 | PC_9 | PC_10 | PF_6 | PB_0 | 4 |PA_2 - Axis 2 Endstop

| **C** | PD_12 | PD_13 | PD_14 | PC_11 | PC_12 | PF_7 | PB_1 | 5 |PA_3 - Axis 3 Endstop

PA_4 - Axis 4 Endstop

**NOTE:** All Alarm A+ pins connect to **+3.3V (P2-37)** shared!PA_5 - Axis 5 Endstop

PA_6 - Axis 6 Endstop

---PA_7 - Axis 7 Endstop

```

## ⚠️ Reserved Pins (DO NOT USE for motion control)

### GPIOC (Alarms)

### Ethernet RMII (DP83848I PHY)```

| Function | Pin | Usage | Header |PC_0 - Axis 0 Alarm

|----------|-----|-------|--------|PC_1 - Axis 1 Alarm

| REF_CLK | PA1 | 50 MHz clock | Reserved |PC_2 - Axis 2 Alarm

| MDIO | PA2 | Management data | Reserved |PC_3 - Axis 3 Alarm

| CRS_DV | PA7 | Carrier sense/RX valid | Reserved |PC_4 - Axis 4 Alarm

| MDC | PC1 | Management clock | Reserved |PC_5 - Axis 5 Alarm

| RXD0 | PC4 | Receive data bit 0 | Not exposed |PC_6 - Axis 6 Alarm

| RXD1 | PC5 | Receive data bit 1 | Not exposed |PC_7 - Axis 7 Alarm

| TX_EN | PG11 | Transmit enable | Reserved |```

| TXD0 | PG12 | Transmit data bit 0 | Reserved |

| TXD1 | PG13 | Transmit data bit 1 | Reserved |### GPIOD (Encoders A/B, Some Step/Dir)

| PHY_RST | PG14 | PHY reset | Reserved |```

PD_0 - Axis 0 Encoder A

### Power & SystemPD_1 - Axis 0 Encoder B

| Pin | Function | Header |PD_2 - Axis 1 Encoder A

|-----|----------|--------|PD_3 - Axis 1 Encoder B

| P2-2 | 5V_IN | Power input |PD_4 - Axis 2 Encoder A

| P2-37, P2-69 | +3.3V | System voltage |PD_5 - Axis 2 Encoder B

| P1-58, P2-1, P2-38, P2-40, P2-70 | GND | Ground |PD_6 - Axis 3 Encoder A

| P1-61 | BOOT0 | Boot mode select |PD_7 - Axis 3 Encoder B

| P2-29 | RESET | System reset |PD_9 - Axis 4 Enable

| P2-8 | VBAT | Battery backup |PD_10 - Axis 5 Enable

| P2-32 | VREF+ | ADC reference |PD_12 - Axis 3 Step

PD_13 - Axis 3 Dir

**⚠️ CRITICAL: Never use these pins for motion control or you will break Ethernet!**PD_14 - Axis 4 Step

PD_15 - Axis 4 Dir

---```



## 🔧 Complete Pin Mapping by Axis### GPIOE (Encoders, Step/Dir)

```

### JOINT 0 - X AXIS (Primary Linear)PE_0 - Axis 1 Step

```PE_1 - Axis 1 Dir

┌─────────────────────────────────────────────────┐PE_2 - Status LED

│  X-Axis - Joint 0 (Primary horizontal movement) │PE_3 - Axis 0 Enable

├─────────────────────────────────────────────────┤PE_4 - Axis 0 Step

│  Stepper Driver:                                │PE_5 - Axis 0 Dir

│    Step:      PE_4    (Base thread, 40 kHz)    │PE_6 - Axis 1 Enable

│    Direction: PE_5                              │PE_7 - Axis 4 Encoder A

│    Enable:    PE_3    (active low)              │PE_8 - Axis 4 Encoder B

│                                                  │PE_9 - Axis 5 Encoder A

│  Encoder Feedback:                              │PE_10 - Axis 5 Encoder B

│    Channel A: PD_0    (quadrature)              │PE_11 - Axis 6 Encoder A

│    Channel B: PD_1    (40 kHz sampling)         │PE_12 - Axis 6 Encoder B

│                                                  │PE_13 - Axis 7 Encoder A

│  Servo Driver Alarm (Differential):             │PE_14 - Axis 7 Encoder B

│    A+ (Alarm+): +3.3V (P2-37, shared)          │```

│    A- (Alarm-): PC_0  (with pullup)            │

│    Data Bit: 16       (txData.inputs)           │### GPIOB (Step/Dir/Enable)

│                                                  │```

│  Limit Switch:                                  │PB_4 - Axis 2 Step

│    Endstop: PA_0      (active low, pullup)      │PB_5 - Axis 2 Dir

│    Data Bit: 0        (txData.inputs)           │PB_8 - Axis 3 Enable

│                                                  │PB_9 - Axis 2 Enable

│  Position Monitor:                              │```

│    Error output: PV[0] (processVariable)        │

│    Fault bit: 8        (txData.inputs)          │### GPIOG (Step/Dir/Enable)

└─────────────────────────────────────────────────┘```

```PG_2 - Axis 5 Step

PG_3 - Axis 5 Dir

### JOINT 1 - Y AXIS (Secondary Linear)PG_4 - Axis 6 Enable

```PG_5 - Axis 6 Step

┌─────────────────────────────────────────────────┐PG_6 - Axis 6 Dir

│  Y-Axis - Joint 1 (Secondary horizontal)        │PG_7 - Axis 7 Enable

├─────────────────────────────────────────────────┤PG_8 - Axis 7 Step

│  Stepper Driver:                                │PG_9 - Axis 7 Dir

│    Step:      PE_0                              │```

│    Direction: PE_1                              │

│    Enable:    PE_6                              │---

│                                                  │

│  Encoder Feedback:                              │## Connector Layout Suggestion

│    Channel A: PD_2                              │

│    Channel B: PD_3                              │### Connector J1 - Axis 0 & 1

│                                                  │```

│  Servo Driver Alarm:                            │Pin 1:  PE_4  - Axis 0 Step

│    A+: +3.3V (shared)                          │Pin 2:  PE_5  - Axis 0 Dir

│    A-: PC_1                                     │Pin 3:  PE_3  - Axis 0 Enable

│    Data Bit: 17                                 │Pin 4:  PD_0  - Axis 0 Enc A

│                                                  │Pin 5:  PD_1  - Axis 0 Enc B

│  Limit Switch:                                  │Pin 6:  PA_0  - Axis 0 Endstop

│    Endstop: PA_1                                │Pin 7:  PC_0  - Axis 0 Alarm

│    Data Bit: 1                                  │Pin 8:  GND

│                                                  │Pin 9:  PE_0  - Axis 1 Step

│  Position Monitor:                              │Pin 10: PE_1  - Axis 1 Dir

│    Error output: PV[1]                          │Pin 11: PE_6  - Axis 1 Enable

│    Fault bit: 9                                 │Pin 12: PD_2  - Axis 1 Enc A

└─────────────────────────────────────────────────┘Pin 13: PD_3  - Axis 1 Enc B

```Pin 14: PA_1  - Axis 1 Endstop

Pin 15: PC_1  - Axis 1 Alarm

### JOINT 2 - Z AXIS (Vertical Linear)Pin 16: GND

``````

┌─────────────────────────────────────────────────┐

│  Z-Axis - Joint 2 (Vertical movement)           │### Connector J2 - Axis 2 & 3

├─────────────────────────────────────────────────┤```

│  Stepper Driver:                                │Pin 1:  PB_4  - Axis 2 Step

│    Step:      PB_4                              │Pin 2:  PB_5  - Axis 2 Dir

│    Direction: PB_5                              │Pin 3:  PB_9  - Axis 2 Enable

│    Enable:    PB_9                              │Pin 4:  PD_4  - Axis 2 Enc A

│                                                  │Pin 5:  PD_5  - Axis 2 Enc B

│  Encoder Feedback:                              │Pin 6:  PA_2  - Axis 2 Endstop

│    Channel A: PD_4                              │Pin 7:  PC_2  - Axis 2 Alarm

│    Channel B: PD_5                              │Pin 8:  GND

│                                                  │Pin 9:  PD_12 - Axis 3 Step

│  Servo Driver Alarm:                            │Pin 10: PD_13 - Axis 3 Dir

│    A+: +3.3V (shared)                          │Pin 11: PB_8  - Axis 3 Enable

│    A-: PC_2                                     │Pin 12: PD_6  - Axis 3 Enc A

│    Data Bit: 18                                 │Pin 13: PD_7  - Axis 3 Enc B

│                                                  │Pin 14: PA_3  - Axis 3 Endstop

│  Limit Switch:                                  │Pin 15: PC_3  - Axis 3 Alarm

│    Endstop: PA_2                                │Pin 16: GND

│    Data Bit: 2                                  │```

│                                                  │

│  Position Monitor:                              │### Connector J3 - Axis 4 & 5

│    Error output: PV[2]                          │```

│    Fault bit: 10                                │Pin 1:  PD_14 - Axis 4 Step

│                                                  │Pin 2:  PD_15 - Axis 4 Dir

│  NOTE: Z-axis typically needs tighter tolerance │Pin 3:  PD_9  - Axis 4 Enable

│        Error Threshold: 1.0mm vs 2.0mm for X/Y │Pin 4:  PE_7  - Axis 4 Enc A

└─────────────────────────────────────────────────┘Pin 5:  PE_8  - Axis 4 Enc B

```Pin 6:  PA_4  - Axis 4 Endstop

Pin 7:  PC_4  - Axis 4 Alarm

### JOINT 3 - A AXIS (Rotary around X)Pin 8:  GND

```Pin 9:  PG_2  - Axis 5 Step

┌─────────────────────────────────────────────────┐Pin 10: PG_3  - Axis 5 Dir

│  A-Axis - Joint 3 (Rotary around X axis)        │Pin 11: PD_10 - Axis 5 Enable

├─────────────────────────────────────────────────┤Pin 12: PE_9  - Axis 5 Enc A

│  Stepper Driver:                                │Pin 13: PE_10 - Axis 5 Enc B

│    Step:      PB_6                              │Pin 14: PA_5  - Axis 5 Endstop

│    Direction: PB_7                              │Pin 15: PC_5  - Axis 5 Alarm

│    Enable:    PB_8                              │Pin 16: GND

│                                                  │```

│  Encoder Feedback:                              │

│    Channel A: PD_6                              │### Connector J4 - Axis 6 & 7

│    Channel B: PD_7                              │```

│                                                  │Pin 1:  PG_5  - Axis 6 Step

│  Servo Driver Alarm:                            │Pin 2:  PG_6  - Axis 6 Dir

│    A+: +3.3V (shared)                          │Pin 3:  PG_4  - Axis 6 Enable

│    A-: PC_3                                     │Pin 4:  PE_11 - Axis 6 Enc A

│    Data Bit: 19                                 │Pin 5:  PE_12 - Axis 6 Enc B

│                                                  │Pin 6:  PA_6  - Axis 6 Endstop

│  Limit Switch:                                  │Pin 7:  PC_6  - Axis 6 Alarm

│    Endstop: PA_3 (optional for rotary)          │Pin 8:  GND

│    Data Bit: 3                                  │Pin 9:  PG_8  - Axis 7 Step

│                                                  │Pin 10: PG_9  - Axis 7 Dir

│  Position Monitor:                              │Pin 11: PG_7  - Axis 7 Enable

│    Error output: PV[3]                          │Pin 12: PE_13 - Axis 7 Enc A

│    Fault bit: 11                                │Pin 13: PE_14 - Axis 7 Enc B

│                                                  │Pin 14: PA_7  - Axis 7 Endstop

│  Units: Degrees (360° = 1 rotation)            │Pin 15: PC_7  - Axis 7 Alarm

│  Typical: 200 steps/rev × microstepping        │Pin 16: GND

└─────────────────────────────────────────────────┘```

```

---

### JOINT 4 - B AXIS (Rotary around Y)

```## Signal Voltage Levels

┌─────────────────────────────────────────────────┐

│  B-Axis - Joint 4 (Rotary around Y axis)        │**All I/O**: 3.3V logic (5V tolerant on most pins, check STM32F407 datasheet)

├─────────────────────────────────────────────────┤

│  Stepper Driver:                                │### Outputs (Step/Dir/Enable):

│    Step:      PC_6                              │- Logic HIGH: 3.3V

│    Direction: PC_7                              │- Logic LOW: 0V

│    Enable:    PC_8                              │- Max current: 25mA per pin

│                                                  │- **Recommendation**: Use optocouplers or level shifters for servo drives

│  Encoder Feedback:                              │

│    Channel A: PC_9                              │### Inputs (Encoders/Endstops/Alarms):

│    Channel B: PC_10                             │- Logic HIGH: 2.0V - 3.3V

│                                                  │- Logic LOW: 0V - 0.8V

│  Servo Driver Alarm:                            │- Input protection: Internal pullup/pulldown available

│    A+: +3.3V (shared)                          │- **Recommendation**: Use series resistors (1kΩ) for protection

│    A-: PF_6                                     │

│    Data Bit: 20                                 │---

│                                                  │

│  Limit Switch:                                  │## Cable Specifications

│    Endstop: PB_0 (optional for rotary)          │

│    Data Bit: 4                                  │### Encoder Cables:

│                                                  │- **Type**: Shielded twisted pair

│  Position Monitor:                              │- **Maximum length**: 5m for single-ended, 30m+ for differential

│    Error output: PV[4]                          │- **Wire gauge**: 22-24 AWG

│    Fault bit: 12                                │- **Shield**: Connect to GND at controller end only

└─────────────────────────────────────────────────┘

```### Step/Dir Cables:

- **Type**: Shielded or unshielded, depending on EMI environment

### JOINT 5 - C AXIS (Rotary around Z / Spindle)- **Maximum length**: 3m recommended for 3.3V signals

```- **Wire gauge**: 24 AWG

┌─────────────────────────────────────────────────┐- **Note**: Use differential drivers (RS-422) for longer distances

│  C-Axis - Joint 5 (Rotary around Z / Spindle)   │

├─────────────────────────────────────────────────┤### Endstop/Alarm Cables:

│  Stepper Driver:                                │- **Type**: Shielded for EMI immunity

│    Step:      PD_12                             │- **Maximum length**: 10m

│    Direction: PD_13                             │- **Wire gauge**: 24-26 AWG

│    Enable:    PD_14                             │

│                                                  │---

│  Encoder Feedback:                              │

│    Channel A: PC_11                             │## Power Distribution

│    Channel B: PC_12                             │

│                                                  │**IMPORTANT**: Provide adequate power supply filtering:

│  Servo Driver Alarm:                            │- Add 100nF ceramic capacitor near each GPIO group

│    A+: +3.3V (shared)                          │- Add 10μF electrolytic capacitor at power entry

│    A-: PF_7                                     │- Separate digital and motor power grounds

│    Data Bit: 21                                 │- Use star grounding topology

│                                                  │

│  Limit Switch:                                  │---

│    Endstop: PB_1 (optional for rotary)          │

│    Data Bit: 5                                  │## Jumper Settings (If Applicable)

│                                                  │

│  Position Monitor:                              │Check your XCORE407I board for:

│    Error output: PV[5]                          │- Pull-up/pull-down resistor jumpers

│    Fault bit: 13                                │- 5V/3.3V level select jumpers

│                                                  │- Encoder power supply jumpers

│  Common use: 4th axis indexer or spindle orient │

└─────────────────────────────────────────────────┘---

```

**Note**: This pin assignment is a suggestion. You can modify it in `config_6axis_closed_loop.json` to match your actual hardware layout.

---

**Created**: October 26, 2025

## 🔌 Alarm Wiring (Differential A+/A-)

### Photoelectric Isolated Alarm Signals

**All servo motors have differential alarm output:**
- **A+ (Alarm+)**: Red/Orange wire
- **A- (Alarm-)**: Black/Blue wire

### Connection Diagram
```
STM32 XCore407I:
    +3.3V (P2-37) ──┬─── Servo 0 (X) Alarm A+
                    ├─── Servo 1 (Y) Alarm A+
                    ├─── Servo 2 (Z) Alarm A+
                    ├─── Servo 3 (A) Alarm A+
                    ├─── Servo 4 (B) Alarm A+
                    └─── Servo 5 (C) Alarm A+
                         (ALL A+ SHARED!)

    PC_0 ──────────────── Servo 0 (X) Alarm A-
    PC_1 ──────────────── Servo 1 (Y) Alarm A-
    PC_2 ──────────────── Servo 2 (Z) Alarm A-
    PC_3 ──────────────── Servo 3 (A) Alarm A-
    PF_6 ──────────────── Servo 4 (B) Alarm A-
    PF_7 ──────────────── Servo 5 (C) Alarm A-
```

### Per-Servo Connection
```
Servo Motor (each):
    A+ ──────────────> +3.3V (shared power pin)
    A- ──────────────> GPIO Pin (unique per servo)
                       with internal pullup enabled

Operation:
    Normal:  A+/A- OPEN   → GPIO reads HIGH (3.3V)
    Alarm:   A+/A- SHORT  → GPIO reads LOW  (0V)
```

### Alarm Bit Mapping
| Joint | Axis | Alarm Pin (A-) | Data Bit | txData.inputs |
|-------|------|----------------|----------|---------------|
| 0 | X | PC_0 | 16 | bit 16 |
| 1 | Y | PC_1 | 17 | bit 17 |
| 2 | Z | PC_2 | 18 | bit 18 |
| 3 | A | PC_3 | 19 | bit 19 |
| 4 | B | PF_6 | 20 | bit 20 |
| 5 | C | PF_7 | 21 | bit 21 |

**⚠️ CRITICAL:** 
- A+ ALWAYS goes to +3.3V (NOT to GPIO!)
- A- goes to GPIO pin with pullup enabled
- JSON config: `"Pullup": true, "Invert": true`

---

## ⚡ Power Pins

### +3.3V Supply
| Header Pin | Usage | Max Current |
|------------|-------|-------------|
| P2-37 | +3.3V output | 200mA total for all alarm A+ |
| P2-69 | +3.3V output | (shared with P2-37) |

**NOTE:** The 6 alarm A+ connections share this supply. Each optocoupler draws ~5-10mA, total ~60mA max.

### Ground
| Header Pin | Usage |
|------------|-------|
| P1-58 | GND |
| P2-1 | GND |
| P2-38 | GND |
| P2-40 | GND |
| P2-70 | GND |

**TIP:** Use multiple GND connections for better noise immunity.

---

## 📊 Data Bit Mapping Summary

### txData.inputs (32-bit word sent to LinuxCNC via Ethernet)

```
Bits 0-7: Endstops
    0: X endstop
    1: Y endstop
    2: Z endstop
    3: A endstop
    4: B endstop
    5: C endstop
    6-7: Reserved

Bits 8-15: Position Monitor Faults
    8:  X position error
    9:  Y position error
    10: Z position error
    11: A position error
    12: B position error
    13: C position error
    14-15: Reserved

Bits 16-23: Motor Alarms (Servo Driver)
    16: X alarm
    17: Y alarm
    18: Z alarm
    19: A alarm
    20: B alarm
    21: C alarm
    22-23: Reserved

Bits 24-31: Reserved for future use
```

---

## 🔓 Available Pins for Expansion

### After 6-axis closed-loop allocation, these pins are still free:

#### Port E (PE) - Many available!
- PE_7, PE_8, PE_9, PE_10, PE_11, PE_12, PE_13, PE_14, PE_15

#### Port F (PF)
- PF_0, PF_1, PF_2, PF_3, PF_4, PF_5, PF_8, PF_9, PF_10, PF_11, PF_12, PF_13, PF_14, PF_15

#### Port G (available after Ethernet reserved)
- PG_0, PG_1, PG_2, PG_3, PG_4, PG_5, PG_6, PG_7, PG_8, PG_9, PG_10, PG_15

#### Port B
- PB_2, PB_3, PB_10, PB_11, PB_12, PB_13, PB_14, PB_15

#### Port D
- PD_8, PD_9, PD_10, PD_11, PD_15

### Suggested Uses for Free Pins:
- **Spindle control**: PWM output (PE_9 has TIM1_CH1)
- **Coolant**: Digital outputs (PE_10, PE_11)
- **Probe input**: Tool touch-off (PE_12)
- **MPG/Handwheel**: Encoder input (PG_2/PG_3)
- **Additional endstops**: Home switches separate from limits
- **Relay outputs**: Mist, flood, air blast, etc.

---

## 📐 Typical Machine Configurations

### 3-Axis Milling Machine
- Use: X, Y, Z (Joints 0-2)
- Leave: A, B, C unused (disable in LinuxCNC INI)

### 4-Axis Milling with Rotary Table
- Use: X, Y, Z, A (Joints 0-3)
- A-axis: Rotary table (0-360°)

### 5-Axis Milling (Nutating Head)
- Use: X, Y, Z, A, B (Joints 0-4)
- A/B: Tilting head axes

### 5-Axis Milling (Trunnion Table)
- Use: X, Y, Z, A, C (Joints 0-2, 3, 5)
- A/C: Table rotation axes
- Disable B (Joint 4) in LinuxCNC

### 6-Axis Robot Arm
- Use: All XYZABC (Joints 0-5)
- Full kinematics in LinuxCNC

---

## 🧪 Testing Checklist

### Per Axis Testing:
- [ ] **Step/Dir**: Jog axis, verify motor turns correct direction
- [ ] **Encoder**: Manually rotate motor, check count in HAL meter
- [ ] **Encoder direction**: Must match step direction (if not, swap A/B)
- [ ] **Position Monitor**: Jog slowly, verify error stays near zero
- [ ] **Alarm**: Disconnect encoder, verify alarm bit sets (bit 16-21)
- [ ] **Endstop**: Trigger limit switch, verify bit sets (bit 0-5)

### Alarm A+/A- Testing:
- [ ] Verify A+ connected to +3.3V (measure ~3.3V with multimeter)
- [ ] Verify A- connected to GPIO pin (measure ~3.3V when no alarm)
- [ ] Trigger alarm: A- should go to ~0V, bit should set
- [ ] Check serial monitor: "ALARM: Joint X alarm triggered!"

---

## 📞 Support & Documentation

- **Full Wiring Guide**: See `ALARM_DIFFERENTIAL_WIRING.md`
- **Quick Reference**: See `ALARM_QUICK_REFERENCE.md`
- **Closed-Loop Setup**: See `ETHERNET_CLOSED_LOOP_GUIDE.md`
- **Main README**: See `README.md`

---

**Created:** October 26, 2025  
**Version:** 2.0 - 6-Axis Configuration  
**Board:** XCore407i (STM32F407IGT6)  
**System:** Closed-Loop Ethernet-based Remora
