# Firmware Configuration Files

This folder contains ready-to-use JSON configuration files for the Remora firmware running on XCORE407I (STM32F407IGT6).

## Available Configurations

| File | Axes | Description |
|------|------|-------------|
| `config_xcore407i_3axis_eth.txt` | XYZ (0-2) | 3-axis CNC mill/router |
| `config_xcore407i_4axis_eth.txt` | XYZU (0-3) | 4-axis with rotary table |
| `config_xcore407i_5axis_eth.txt` | XYZUV (0-4) | 5-axis tilting/nutating head |
| `config_xcore407i_6axis_eth.txt` | XYZUVW (0-5) | Full 6-axis robot/mill |

## What These Files Define

Each JSON configuration specifies:

1. **Communication settings**
   - IP address: `192.168.10.10`
   - Port: `27181`
   - Thread frequency: `120000` Hz (120 kHz base thread)

2. **Motor control pins** (enable, step, direction) for each joint
   - Configurable invert flags per signal
   - Velocity mode for closed-loop control

3. **Encoder inputs** (A/B channels, optional index)
   - Position feedback for each axis
   - Process variable (PV) index assignment

4. **Digital inputs**
   - Endstop/limit switches (one per axis)
   - Alarm signals (one per axis)
   - Configurable pullup and invert settings

## Pin Assignment Strategy

All pins are chosen to:
- ✅ Be accessible on board headers P1/P2
- ✅ Avoid Ethernet RMII signals (PA2, PA7, PC1, PC4, PC5, PG11, PG13, PG14)
- ✅ Use consistent port groupings for clean wiring

### Pin Mapping Summary

#### Motors (Enable/Step/Dir)
- X: PE3/PE4/PE5
- Y: PE6/PE0/PE1
- Z: PB9/PB4/PB5
- U: PE8/PE9/PE10
- V: PE11/PE12/PE13
- W: PB6/PB7/PB8

#### Encoders (A/B channels)
- X: PD0/PD1
- Y: PD2/PD3
- Z: PD4/PD5
- U: PD6/PD7
- V: PD8/PD9
- W: PD10/PD11

#### Endstops (digital inputs 0,1,2,6,7,8)
- X: PF11 → remora.input.0
- Y: PF13 → remora.input.1
- Z: PF14 → remora.input.2
- U: PF15 → remora.input.6
- V: PH8 → remora.input.7
- W: PH10 → remora.input.8

#### Alarms (digital inputs 3,4,5,9,10,11)
- X: PF12 → remora.input.3
- Y: PH6 → remora.input.4
- Z: PH7 → remora.input.5
- U: PH5 → remora.input.9
- V: PH9 → remora.input.10
- W: PH11 → remora.input.11

#### Touch Probe (digital input 12)
- Probe sensor: PE14 (P2-59) → remora.input.12
- Configured for Normally Open (NO) probe by default
- Change `"invert": 1` in JSON for Normally Closed (NC) probe

## Usage

### 1. Choose Your Configuration

Pick the file matching your axis count (3, 4, 5, or 6).

### 2. Flash to Board

The firmware must load this configuration at boot. Methods vary by firmware version:
- Some versions read from SD card
- Others embed the config in flash
- Consult your firmware documentation for specifics

### 3. Use Matching LinuxCNC Config

Each firmware JSON has a corresponding HAL/INI pair in `../linuxcnc/`:
- `config_xcore407i_3axis_eth.txt` → `xcore407i_3axis.hal` / `.ini`
- `config_xcore407i_4axis_eth.txt` → `xcore407i_4axis.hal` / `.ini`
- And so on...

## Customization

To modify pin assignments:

1. Edit the JSON (use VS Code or any text editor)
2. Change `"pin"` values to match your wiring
3. Ensure new pins are:
   - Not used by Ethernet RMII
   - Accessible on your board headers
   - Compatible with peripheral function (GPIO/TIM/encoder)
4. Update the corresponding LinuxCNC HAL if you change input indices

## Validation

After editing, validate your JSON:

**PowerShell:**
```powershell
Get-Content config_xcore407i_3axis_eth.txt
```

**Python:**
```bash
# JSON validator no longer needed; format is Modules-based and consumed by firmware directly
```

If no errors, your JSON is valid!

## Schematic Reference

Consult `schematic xcore407i/` for detailed board pin assignments and to verify physical connectivity.

---

**Questions?** See `../linuxcnc/README.md` for complete integration guide.
