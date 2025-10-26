# Getting Started with 8-Axis Closed-Loop System

## Quick Start Guide

This guide will help you get your XCORE407I board running with closed-loop motor control in under 30 minutes.

---

## What You Have

Your XCORE407I Remora firmware now includes:

✅ **8 Encoder Modules** - Quadrature position feedback  
✅ **8 Endstop Modules** - Home and limit switches  
✅ **8 Alarm Modules** - Motor error detection  
✅ **8 Stepgen Modules** - Step/direction outputs  
✅ **Complete Examples** - Ready-to-use configurations  

---

## Prerequisites

### Hardware Needed
- XCORE407I board (STM32F407IGT6)
- Ethernet cable and network connection
- Closed-loop servo drives (or stepper drivers with encoders)
- Encoders (quadrature A/B, optionally with Index/Z)
- Limit/home switches (NC or NO type)
- Motor alarm connections (if available on drives)

### Software Needed
- PlatformIO (for building firmware)
- LinuxCNC 2.8+ (with remora-eth component)
- dfu-util (for flashing via USB DFU)

---

## 5-Step Quick Start

### Step 1: Choose Your Configuration

Pick the example that matches your machine:

| Configuration | Best For | File |
|--------------|----------|------|
| **1-Axis Test** | Initial testing, single motor | `examples/config_1axis_test.json` |
| **3-Axis Basic** | CNC mills, routers (X,Y,Z) | `examples/config_3axis_basic.json` |
| **6-Axis** | Multi-axis machines, robots | `examples/config_6axis_with_index.json` |
| **8-Axis Full** | Maximum capability | `examples/config_8axis_closed_loop.json` |

**Recommendation:** Start with `config_1axis_test.json` to verify everything works!

### Step 2: Build and Flash Firmware

```bash
# Build the firmware
cd /path/to/remora-xcore407i
pio run -e xcore407i_eth_dfu

# Put board in DFU mode:
# 1. Press and hold BOOT0 button
# 2. Press and release RESET button
# 3. Release BOOT0 button

# Flash the firmware
pio run -e xcore407i_eth_dfu -t upload

# Or use dfu-util directly:
dfu-util -a 0 -s 0x08000000:leave -D .pio/build/xcore407i_eth_dfu/firmware.bin
```

### Step 3: Upload Configuration

Copy your chosen JSON config to the board:
- Via SD card (if available)
- Via Ethernet upload
- Via UART terminal

**Note:** The default configuration is embedded in firmware. For custom configs, see the Remora documentation for uploading JSON files.

### Step 4: Connect Hardware

#### Wiring Example (Axis 0 - X):

**Step/Direction (to servo drive):**
- PE_3 → Enable
- PE_4 → Step
- PE_5 → Direction

**Encoder (from motor/encoder):**
- PD_0 → Channel A
- PD_1 → Channel B
- PD_8 → Index/Z (optional)
- GND → Encoder ground

**Endstop (limit switch):**
- PA_0 → Switch signal
- GND → Switch ground
- (Internal pullup enabled if `"Pullup": true`)

**Alarm (from servo drive):**
- PC_0 → Alarm signal
- GND → Alarm ground
- (Internal pullup enabled if `"Pullup": true`)

**See `examples/README.md` for complete pin tables!**

### Step 5: Configure LinuxCNC

Create a HAL file (e.g., `custom.hal`):

```hal
# Load Remora component
loadrt remora-eth board_ip=192.168.10.10

# Load PID for closed loop (one per axis)
loadrt pid names=j0pid

# Add functions to threads
addf remora-eth.read servo-thread
addf j0pid.do-pid-calcs servo-thread
addf remora-eth.write servo-thread

# Set encoder scale (example: 2000 CPR encoder, 5mm pitch ballscrew)
setp remora.joint.0.encoder-scale 400

# Set step scale (example: 10000 steps/rev, 5mm pitch)
setp remora.joint.0.scale 2000

# Configure PID
setp j0pid.Pgain 30.0
setp j0pid.Igain 0.0
setp j0pid.Dgain 0.0
setp j0pid.FF1 1.0
setp j0pid.maxoutput 1000

# Connect enable
net j0-enable <= joint.0.amp-enable-out => remora.joint.0.enable
net j0-enable => j0pid.enable

# Connect position feedback (encoder → PID → joint)
net j0-pos-fb <= remora.joint.0.encoder-count => j0pid.feedback
net j0-pos-fb => joint.0.motor-pos-fb

# Connect position command (joint → PID)
net j0-pos-cmd <= joint.0.motor-pos-cmd => j0pid.command

# Connect PID output to velocity command
net j0-vel-cmd <= j0pid.output => remora.joint.0.vel-cmd

# Connect endstop
net j0-home <= remora.input.0 => joint.0.home-sw-in
net j0-limit <= remora.input.0 => joint.0.neg-lim-sw-in
net j0-limit => joint.0.pos-lim-sw-in

# Connect alarm to E-stop
net j0-alarm <= remora.input.8 => motion.digital-in-00
```

---

## Testing Procedure

### 1. Power-On Test
```bash
# Check status LED blinks at 2 Hz
# This confirms firmware is running
```

### 2. Network Test
```bash
# Ping the board
ping 192.168.10.10

# Should see responses if network is configured correctly
```

### 3. Encoder Test
```bash
# In LinuxCNC, open HAL Meter
halmeter

# Select: remora.joint.0.encoder-count
# Manually rotate motor shaft
# Verify count changes (positive/negative)
```

### 4. Endstop Test
```bash
# In HAL Meter, select: remora.input.0
# Trigger limit switch
# Verify value changes (0 → 1 or 1 → 0)
```

### 5. Alarm Test
```bash
# In HAL Meter, select: remora.input.8
# Trigger alarm on servo drive (if available)
# Verify alarm bit changes
# Check console for alarm message
```

### 6. Motion Test
```bash
# Enable machine in LinuxCNC
# Jog axis slowly (F10 or jog controls)
# Verify:
#   - Motor moves smoothly
#   - Encoder count matches movement
#   - Following error is small (<0.5mm)
```

---

## Common Issues and Solutions

### Issue: "Encoder not counting"
**Solution:**
1. Check A/B wiring (may need to swap)
2. Verify encoder has power
3. Check joint is enabled in LinuxCNC
4. Test with `halmeter remora.joint.0.encoder-count`

### Issue: "Alarm always active"
**Solution:**
1. Check `"Invert"` setting in JSON (try opposite value)
2. Verify servo drive is powered and not faulted
3. Check alarm signal wiring
4. Some drives need external pullup on alarm signal

### Issue: "Endstop not detected"
**Solution:**
1. Verify switch type matches `"Invert"` setting:
   - NC switch: `"Invert": true`
   - NO switch: `"Invert": false`
2. Check `"Pullup": true` if no external resistor
3. Test with multimeter - switch should change pin voltage

### Issue: "Motor runs away" or "Following error"
**Solution:**
1. Check encoder direction matches step direction
   - If opposite: swap A/B channels OR invert direction pin
2. Verify encoder scale matches step scale
3. Reduce P gain initially (try 10.0)
4. Set FF1 = 1.0 (velocity feedforward)
5. Increase FERROR limit temporarily while tuning

### Issue: "Firmware won't flash"
**Solution:**
1. Verify board is in DFU mode (BOOT0 held during reset)
2. Check USB cable (must support data, not just charging)
3. Try different USB port
4. Install STM32 DFU drivers (Windows)
5. Use ST-Link if DFU fails

---

## Tuning Tips

### Encoder Scaling
```ini
[JOINT_0]
# Calculate: encoder_CPR / ballscrew_pitch
# Example: 2000 CPR encoder, 5mm pitch = 400 counts/mm
ENCODER_SCALE = 400
```

### Step Scaling
```ini
[JOINT_0]
# Calculate: (steps_per_rev * microstepping) / ballscrew_pitch
# Example: 200 steps/rev, 10x microstepping, 5mm pitch = 400 steps/mm
SCALE = 400
```

### PID Tuning (Quick Method)
1. **Start conservative:**
   - P = 10, I = 0, D = 0, FF1 = 1.0
2. **Increase P** until oscillation starts, then reduce by 50%
3. **Add I** if steady-state error exists (usually not needed)
4. **Add D** if high-speed instability (careful, can amplify noise)
5. **Adjust FF1** to minimize following error (typically 0.9-1.1)

---

## Next Steps

### For Testing:
1. Use `config_1axis_test.json`
2. Verify each feature independently
3. Once working, add more axes

### For Production:
1. Choose appropriate multi-axis config
2. Customize pin assignments if needed
3. Document your specific setup
4. Create backup of working configuration

### For Advanced Features:
1. Add index pulse for absolute positioning
2. Implement separate home and limit switches
3. Configure alarm handling in LinuxCNC
4. Add analog inputs for load monitoring

---

## Additional Resources

- **Detailed Setup:** `CLOSED_LOOP_SETUP.md`
- **Implementation Details:** `IMPLEMENTATION_SUMMARY.md`
- **Pin Reference:** `PIN_ASSIGNMENT.md`
- **Examples Guide:** `examples/README.md`
- **Verification Report:** `VERIFICATION.md`

### External Links
- **Remora Project:** https://github.com/scottalford75/Remora
- **LinuxCNC Docs:** http://linuxcnc.org/docs/html/
- **LinuxCNC Forum:** https://forum.linuxcnc.org/

---

## Support

If you encounter issues:

1. Check the troubleshooting sections in this guide
2. Review the comprehensive documentation files
3. Search LinuxCNC forum for similar issues
4. Post detailed description with:
   - Configuration file used
   - HAL file excerpt
   - Error messages or symptoms
   - Hardware details

---

## Success Checklist

Before considering your setup complete:

- [ ] Firmware flashes successfully
- [ ] Network ping responds (192.168.10.10)
- [ ] Status LED blinks
- [ ] Encoder counts in both directions
- [ ] Endstops detected correctly
- [ ] Alarm inputs work (if connected)
- [ ] Machine enables in LinuxCNC
- [ ] Smooth jogging in both directions
- [ ] Following error < 1mm at normal speed
- [ ] Homing sequence works
- [ ] E-stop functions correctly

---

**Ready to start? Begin with `config_1axis_test.json` and work through the testing procedure!**

Good luck with your closed-loop system! 🎯
