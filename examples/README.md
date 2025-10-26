# Configuration Examples for XCORE407I Remora Firmware

This directory contains example configuration files for different machine setups using the XCORE407I board with Remora firmware.

## Available Configurations

### 1. `config_3axis_basic.json`
**3-Axis Basic Setup (X, Y, Z)**
- Suitable for basic 3-axis CNC mills or routers
- Includes:
  - 3x Stepgen modules (step/direction output)
  - 3x Encoder modules (position feedback)
  - 3x Endstop modules (homing and limits)
  - 3x Alarm modules (motor error detection)
- Recommended for: Entry-level closed-loop systems

### 2. `config_6axis_with_index.json`
**6-Axis Configuration with Index Pulse (X, Y, Z, A, B, C)**
- Suitable for 6-axis machines (3 linear + 3 rotary)
- Includes:
  - 6x Stepgen modules
  - 6x Encoder modules (Joint 0 includes Index/Z pulse)
  - 6x Endstop modules
  - 6x Alarm modules
- Recommended for: Advanced multi-axis machines, robotic arms

### 3. `config_6axis_closed_loop.json`
**6-Axis Full Configuration (X, Y, Z, A, B, C)**
- Maximum axis configuration
- Includes:
  - 6x Stepgen modules
  - 6x Encoder modules
  - 6x Endstop modules
  - 6x Alarm modules
- Recommended for: Complex machines, dual gantry systems, multi-spindle setups

## Module Types Explained

### Stepgen Module
Generates step and direction pulses for motor drivers.

**Parameters:**
- `Joint Number` - Joint index (0-7)
- `Enable Pin` - Motor enable output
- `Step Pin` - Step pulse output
- `Direction Pin` - Direction output
- `Step Bit` - DDS accumulator bit (typically 22)

### Encoder Module
Reads quadrature encoder signals for position feedback.

**Parameters:**
- `Joint Number` - Joint index (0-7)
- `Channel A Pin` - Encoder A phase input
- `Channel B Pin` - Encoder B phase input
- `Index Pin` - (Optional) Encoder Z/index pulse for absolute reference

**Features:**
- Quadrature decoding (4x resolution)
- Automatic enable/disable with joint
- Index pulse support for absolute positioning
- Feedback via `txData.jointFeedback[N]`

### Endstop Module
Monitors limit and home switches.

**Parameters:**
- `Pin` - Digital input pin
- `Data Bit` - Bit position in inputs word (0-7 for endstops)
- `Invert` - Reverse logic (true for NC switches, false for NO)
- `Pullup` - Enable internal pullup resistor

**Features:**
- Software debouncing (3 samples)
- Configurable active logic
- Feedback via `txData.inputs` bits 0-7

### Alarm Module
Monitors servo drive error/alarm signals.

**Parameters:**
- `Joint Number` - Associated joint index
- `Pin` - Digital input pin
- `Data Bit` - Bit position in inputs word (8-15 for alarms)
- `Invert` - Reverse logic (typically true for active-low alarms)
- `Pullup` - Enable internal pullup resistor

**Features:**
- Software debouncing
- Alarm event counting
- Console logging on alarm
- Feedback via `txData.inputs` bits 8-15

## Pin Mapping Summary

### Stepgen Pins (8 axes available)
| Axis | Enable | Step  | Direction |
|------|--------|-------|-----------|
| 0 (X)| PE_3   | PE_4  | PE_5      |
| 1 (Y)| PE_6   | PE_0  | PE_1      |
| 2 (Z)| PB_9   | PB_4  | PB_5      |
| 3 (A)| PB_8   | PD_12 | PD_13     |
| 4 (B)| PD_9   | PD_14 | PD_15     |
| 5 (C)| PD_10  | PG_2  | PG_3      |
| 6 (U)| PG_4   | PG_5  | PG_6      |
| 7 (V)| PG_7   | PG_8  | PG_9      |

### Encoder Pins (8 axes available)
| Axis | Channel A | Channel B | Index (Optional) |
|------|-----------|-----------|------------------|
| 0 (X)| PD_0      | PD_1      | PD_8             |
| 1 (Y)| PD_2      | PD_3      | -                |
| 2 (Z)| PD_4      | PD_5      | -                |
| 3 (A)| PD_6      | PD_7      | -                |
| 4 (B)| PE_7      | PE_8      | -                |
| 5 (C)| PE_9      | PE_10     | -                |
| 6 (U)| PE_11     | PE_12     | -                |
| 7 (V)| PE_13     | PE_14     | -                |

### Endstop Pins (bits 0-7)
| Axis | Pin  | Data Bit |
|------|------|----------|
| 0 (X)| PA_0 | 0        |
| 1 (Y)| PA_1 | 1        |
| 2 (Z)| PA_2 | 2        |
| 3 (A)| PA_3 | 3        |
| 4 (B)| PA_4 | 4        |
| 5 (C)| PA_5 | 5        |
| 6 (U)| PA_6 | 6        |
| 7 (V)| PA_7 | 7        |

### Alarm Pins (bits 8-15)
| Axis | Pin  | Data Bit |
|------|------|----------|
| 0 (X)| PC_0 | 8        |
| 1 (Y)| PC_1 | 9        |
| 2 (Z)| PC_2 | 10       |
| 3 (A)| PC_3 | 11       |
| 4 (B)| PC_4 | 12       |
| 5 (C)| PC_5 | 13       |
| 6 (U)| PC_6 | 14       |
| 7 (V)| PC_7 | 15       |

## Customizing Configuration

### Adding an Encoder with Index Pulse
```json
{
  "Thread": "Base",
  "Type": "Encoder",
  "Comment": "Joint 0 with Index",
  "Joint Number": 0,
  "Channel A Pin": "PD_0",
  "Channel B Pin": "PD_1",
  "Index Pin": "PD_8"
}
```

### Changing Endstop Logic
For Normally Open (NO) switches:
```json
{
  "Invert": false,
  "Pullup": true
}
```

For Normally Closed (NC) switches:
```json
{
  "Invert": true,
  "Pullup": true
}
```

### Disabling Pullups
If using external pullup/pulldown resistors:
```json
{
  "Pullup": false
}
```

## Usage

1. **Choose your configuration** based on your machine's axis count
2. **Copy the config file** to your SD card or flash memory
3. **Modify pin assignments** if your wiring differs
4. **Flash the firmware** to the XCORE407I board
5. **Configure LinuxCNC** HAL file to match the configuration

## Data Structure

### RX Data (LinuxCNC → STM32)
```cpp
rxData.jointFreqCmd[0-7]   // Velocity commands (32-bit signed int)
rxData.jointEnable         // 8-bit enable mask (bit per joint)
```

### TX Data (STM32 → LinuxCNC)
```cpp
txData.jointFeedback[0-7]  // Encoder counts (32-bit signed int)
txData.inputs              // 16-bit status word:
                          //   bits 0-7:  Endstop states
                          //   bits 8-15: Alarm states
```

## Troubleshooting

### Encoder Not Counting
1. Check A/B channel connections
2. Verify pin assignments in JSON
3. Check if joint is enabled in LinuxCNC
4. Test with `halmeter remora.joint.N.encoder-count`

### Alarm Always Active
1. Check `Invert` setting (should match drive's alarm polarity)
2. Verify alarm signal is not open-collector without pullup
3. Check drive's alarm configuration/status LEDs

### Endstop Not Detected
1. Verify `Invert` matches switch type (NO vs NC)
2. Check `Pullup` is enabled if no external resistor
3. Test with `halmeter remora.input.N`

### Wrong Direction
1. Swap encoder A/B channels, or
2. Change sign of SCALE in INI file, or
3. Invert direction pin logic

## LinuxCNC HAL Example

```hal
# Load Remora component
loadrt remora-eth

# Configure encoder scaling (example: 2000 CPR, 5mm pitch)
setp remora.joint.0.encoder-scale 400

# Connect position feedback
net j0-pos-fb <= remora.joint.0.encoder-count => joint.0.motor-pos-fb

# Connect endstop
net j0-home <= remora.input.0 => joint.0.home-sw-in

# Connect alarm to E-stop chain
net j0-alarm <= remora.input.8 => estop-fault.in-00
```

## Additional Resources

- **Main Documentation**: `../CLOSED_LOOP_SETUP.md`
- **Implementation Details**: `../IMPLEMENTATION_SUMMARY.md`
- **Pin Reference**: `../PIN_ASSIGNMENT.md`
- **Quick Start Guide**: `../QUICKSTART.md`

## License

These configuration examples are part of the Remora project and distributed under GPL v2.

## Support

For issues, questions, or contributions:
- GitHub Repository: https://github.com/Marioskiv/remora-xcore407i
- Remora Project: https://github.com/scottalford75/Remora
- LinuxCNC Forum: https://forum.linuxcnc.org/
