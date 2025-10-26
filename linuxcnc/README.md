# LinuxCNC Configuration for XCORE407I Remora

Complete LinuxCNC configuration files for 3-axis closed-loop CNC control with Remora Ethernet.

---

## 📁 Files Included

| File | Purpose |
|------|---------|
| `xcore407i_3axis.ini` | 3-axis INI (XYZ) |
| `xcore407i_3axis.hal` | 3-axis HAL (Remora wiring) |
| `xcore407i_4axis.ini` | 4-axis INI (XYZU) |
| `xcore407i_4axis.hal` | 4-axis HAL (adds U axis) |
| `xcore407i_5axis.ini` | 5-axis INI (XYZUV) |
| `xcore407i_5axis.hal` | 5-axis HAL (adds U,V axes) |
| `xcore407i_6axis.ini` | 6-axis INI (XYZUVW) |
| `xcore407i_6axis.hal` | 6-axis HAL (adds U,V,W axes) |
| `custom_postgui.hal` | Post-GUI connections (buttons, indicators) |
| `README.md` | This file |

---

## 🚀 Quick Installation

### 1. Install Remora Component

```bash
# Clone Remora repository
cd ~
git clone https://github.com/scottalford75/Remora.git
cd Remora/LinuxCNC/Components

# Compile and install
sudo halcompile --install remora-eth.c
```

### 2. Flash Firmware with Configuration

**IMPORTANT:** The hardware pin assignments are defined in the JSON configuration file that must be loaded into the XCORE407I board firmware!

Choose the configuration matching your axis count:

```bash
# For 3-axis (XYZ):
copy resources\config_xcore407i_3axis_eth.txt config.txt

# For 4-axis (XYZU):
copy resources\config_xcore407i_4axis_eth.txt config.txt

# For 5-axis (XYZUV):
copy resources\config_xcore407i_5axis_eth.txt config.txt

# For 6-axis (XYZUVW):
copy resources\config_xcore407i_6axis_eth.txt config.txt

# Flash the configuration to the board (method depends on your setup)
# See firmware documentation for details
```

### 3. Copy LinuxCNC Configuration

```bash
# Create LinuxCNC config directory
mkdir -p ~/linuxcnc/configs/xcore407i

# Copy the HAL/INI files you want (pick one axis count)
cp xcore407i_3axis.ini ~/linuxcnc/configs/xcore407i/
cp xcore407i_3axis.hal ~/linuxcnc/configs/xcore407i/
# or
# cp xcore407i_4axis.ini ~/linuxcnc/configs/xcore407i/
# cp xcore407i_4axis.hal ~/linuxcnc/configs/xcore407i/
# or
# cp xcore407i_5axis.ini ~/linuxcnc/configs/xcore407i/
# cp xcore407i_5axis.hal ~/linuxcnc/configs/xcore407i/
# or
# cp xcore407i_6axis.ini ~/linuxcnc/configs/xcore407i/
# cp xcore407i_6axis.hal ~/linuxcnc/configs/xcore407i/
cp custom_postgui.hal ~/linuxcnc/configs/xcore407i/

# Create var file
touch ~/linuxcnc/configs/xcore407i/xcore407i.var
```

### 3. Configure Network

```bash
# Set your PC's Ethernet interface
sudo ifconfig eth0 192.168.10.1 netmask 255.255.255.0

# Test connection
ping 192.168.10.10
```

### 4. Launch LinuxCNC

```bash
cd ~/linuxcnc/configs/xcore407i
# Choose one:
linuxcnc xcore407i_3axis.ini
# linuxcnc xcore407i_4axis.ini
# linuxcnc xcore407i_5axis.ini
# linuxcnc xcore407i_6axis.ini
```

---

## ⚙️ Configuration Details

### Pin Assignments

**IMPORTANT:** Hardware pins are defined in firmware configuration JSON files which must be loaded into the Remora firmware. The HAL file only connects the Remora component to LinuxCNC.

**Firmware config files:**
- 3-axis: `resources/config_xcore407i_3axis_eth.txt` (rename to `config.txt` before upload)
- 4-axis: `resources/config_xcore407i_4axis_eth.txt` (rename to `config.txt` before upload)
- 5-axis: `resources/config_xcore407i_5axis_eth.txt` (rename to `config.txt` before upload)
- 6-axis: `resources/config_xcore407i_6axis_eth.txt` (rename to `config.txt` before upload)

All pins are exposed on board headers P1/P2 and avoid conflicts with Ethernet RMII signals (PA2/PA7, PC1/PC4/PC5, PG11/PG13/PG14).

#### Complete Pin Mapping Table

| Axis | Joint | Enable | Step  | Dir   | Enc A | Enc B | Endstop (input) | Alarm (input) | Header Locations |
|------|-------|--------|-------|-------|-------|-------|-----------------|---------------|------------------|
| X    | 0     | PE3    | PE4   | PE5   | PD0   | PD1   | PF11 (idx 0)    | PF12 (idx 3)  | P2-4,5,6 / P1-41,42 / P2-45,48 |
| Y    | 1     | PE6    | PE0   | PE1   | PD2   | PD3   | PF13 (idx 1)    | PH6 (idx 4)   | P2-7 / P1-64,65 / P1-43,44 / P2-47,61 |
| Z    | 2     | PB9    | PB4   | PB5   | PD4   | PD5   | PF14 (idx 2)    | PH7 (idx 5)   | P1-63,57,† / P1-45,46 / P2-50,64 |
| U    | 3     | PE8    | PE9   | PE10  | PD6   | PD7   | PF15 (idx 6)    | PH5 (idx 9)   | P2-53,56,55 / P1-47,48 / P2-49,42 |
| V    | 4     | PE11   | PE12  | PE13  | PD8   | PD9   | PH8 (idx 7)     | PH9 (idx 10)  | P2-58,57,60 / P1-4,5 / P2-63,66 |
| W    | 5     | PB6    | PB7   | PB8   | PD10  | PD11  | PH10 (idx 8)    | PH11 (idx 11) | P1-59,60,62 / P1-6,7 / P2-65,67 |

**Notes:**
- † PB5 location varies by board revision; check your specific schematic
- All encoder pairs use quadrature mode (no index pulse by default)
- Endstops/alarms use internal pullups; configure `invert` in firmware JSON if needed
- Digital input indices match the HAL file conventions (0-2 for XYZ endstops, 6-8 for UVW endstops, 3-5/9-11 for alarms)
- **Touch probe**: PE14 (P2-59) → `remora.input.12` → `motion.probe-input`

### Touch Probe

All configurations include touch probe support on **PE14** (header P2-59):

```hal
# Touch probe connection (already configured in HAL files)
net probe-in remora.input.12 => motion.probe-input
```

Use G38.x commands for probing operations:
- **G38.2** - Probe toward workpiece, stop on contact, signal error if probe doesn't trip
- **G38.3** - Probe toward workpiece, stop on contact
- **G38.4** - Probe away from workpiece, stop on loss of contact, signal error if probe doesn't open
- **G38.5** - Probe away from workpiece, stop on loss of contact

**Probe sensor wiring:**
- Normally Open (NO) probe: Set `"invert": 0` in firmware JSON (default)
- Normally Closed (NC) probe: Set `"invert": 1` in firmware JSON

### Scaling Configuration

**Default values assume:**
- **Encoder:** 2000 CPR quadrature
- **Ballscrew:** 5mm pitch
- **Steps:** 200 steps/rev with 10x microstepping

**Calculation:**
```
ENCODER_SCALE = 2000 CPR / 5mm = 400 counts/mm
SCALE = (200 steps/rev × 10 micro) / 5mm = 400 steps/mm
```

**Adjust for your hardware:**
```ini
[JOINT_0]
ENCODER_SCALE = [your_encoder_CPR] / [your_pitch_mm]
SCALE = ([steps_per_rev] × [microstepping]) / [your_pitch_mm]
```

---

## 🎛️ PID Tuning

### Starting Values (Already in INI)

```ini
P = 50.0      # Position gain
I = 0.0       # Integral gain (usually not needed)
D = 0.0       # Derivative gain (can cause instability)
FF0 = 0.0     # Velocity-independent offset
FF1 = 1.0     # Velocity feedforward (important!)
FF2 = 0.0     # Acceleration feedforward
```

### Tuning Procedure

1. **Start Conservative**
   ```
   P = 10.0
   I = 0.0
   D = 0.0
   FF1 = 1.0
   ```

2. **Increase P Gradually**
   - Jog axis slowly
   - Increase P until oscillation starts
   - Reduce P by 50%

3. **Adjust FF1**
   - Watch following error
   - FF1 = 1.0 is ideal
   - Adjust ±10% if needed

4. **Add I if Necessary** (usually not needed)
   - Only if steady-state error exists
   - Start with I = 0.1
   - Too much I causes instability

5. **Fine-tune FERROR**
   ```ini
   FERROR = 5.0       # Max following error (mm)
   MIN_FERROR = 1.0   # Steady-state limit
   ```

### Live Tuning

```bash
# Use halshow to adjust in real-time
halshow

# Or use halcmd
halcmd setp pid_x.Pgain 60.0
```

---

## 🏠 Homing Setup

### Current Configuration

```ini
HOME_SEARCH_VEL = 10.0     # Fast search speed
HOME_LATCH_VEL = 2.0       # Slow latch speed
HOME_SEQUENCE = 1          # Homing order
HOME_USE_INDEX = NO        # Don't use encoder index
```

### To Enable Index Homing

Change in INI:
```ini
HOME_USE_INDEX = YES
```

And in HAL, ensure index pulse is connected:
```hal
# Already configured in Remora firmware
# Index pulse on PD8, PD9, PD10 for X, Y, Z
```

---

## 🐛 Troubleshooting

### "Joint following error"

**Cause:** Encoder direction opposite to step direction

**Solution:**
```hal
# Invert encoder scale (change sign)
setp remora.joint.0.encoder-scale -400.0
```

Or invert step direction in hardware.

### "Can't connect to board"

**Check:**
```bash
ping 192.168.10.10
```

If fails:
1. Check Ethernet cable
2. Verify PC IP: `ifconfig eth0`
3. Check firmware LED blinks (2 Hz)

### "PID runaway"

**Cause:** P gain too high or encoder noise

**Solution:**
1. Reduce P gain by 50%
2. Check encoder wiring
3. Add shielding if noise present

### "Machine won't enable"

**Check:**
1. E-stop not pressed
2. Servo alarms not triggered: `halshow` → `remora.input.3/4/5`
3. Following error within limits

---

## 📊 Monitoring

### Watch Encoder Position

```bash
halmeter
# Select: remora.joint.0.encoder-count
```

### Watch Following Error

```bash
halmeter
# Select: pid_x.error
```

### Watch PID Output

```bash
halmeter
# Select: pid_x.output
```

---

## 🔧 Advanced Configuration

### Available axis configurations

- **3-axis (XYZ)**: `xcore407i_3axis.(hal|ini)` + `resources/config_xcore407i_3axis_eth.txt`
- **4-axis (XYZU)**: `xcore407i_4axis.(hal|ini)` + `remora_config_xcore407i_4axis_eth.json`
- **5-axis (XYZUV)**: `xcore407i_5axis.(hal|ini)` + `remora_config_xcore407i_5axis_eth.json`
- **6-axis (XYZUVW)**: `xcore407i_6axis.(hal|ini)` + `remora_config_xcore407i_6axis_eth.json`

**Input index convention** (firmware must map physical pins accordingly):
- Endstops: X=remora.input.0, Y=1, Z=2, U=6, V=7, W=8
- Alarms:   X=remora.input.3, Y=4, Z=5, U=9, V=10, W=11

All configurations use:
- Velocity mode closed-loop control (ctrl_type=v)
- PID per joint with position feedback from encoders
- Thread frequency: 120 kHz (firmware) / 1 kHz servo thread (LinuxCNC)
- E-stop loop integrated with alarm inputs

### Spindle Control

Uncomment in HAL:
```hal
net spindle-on <= spindle.0.on => remora.sp.enable
net spindle-speed <= spindle.0.speed-out => remora.sp.speed-cmd
```

### Touch Probe

Uncomment in HAL:
```hal
net probe-in <= remora.input.6 => motion.probe-input
```

---

## 📚 Additional Resources

- **LinuxCNC Docs:** http://linuxcnc.org/docs/html/
- **Remora Project:** https://github.com/scottalford75/Remora
- **Forum Support:** https://forum.linuxcnc.org/

---

## ⚠️ Important Notes

1. **Test in simulation first!**
   ```bash
   linuxcnc -l xcore407i_3axis.ini
   ```

2. **Adjust travel limits** in INI to match your machine

3. **Check encoder direction** before running at high speed

4. **Keep FERROR conservative** during initial testing

5. **Have E-stop accessible** at all times!

---

## 📝 Customization Checklist

Before first run:

- [ ] Update `board_ip` in HAL if different
- [ ] Adjust `ENCODER_SCALE` for your encoders
- [ ] Adjust `SCALE` for your steps/mm
- [ ] Set correct `MIN_LIMIT` and `MAX_LIMIT`
- [ ] Tune `MAX_VELOCITY` for your machine
- [ ] Configure `HOME_OFFSET` after finding home position
- [ ] Test emergency stop functionality
- [ ] Verify all alarm inputs work

---

**Ready to run?**

```bash
cd ~/linuxcnc/configs/xcore407i
linuxcnc xcore407i_3axis.ini
```

Good luck! 🎯
