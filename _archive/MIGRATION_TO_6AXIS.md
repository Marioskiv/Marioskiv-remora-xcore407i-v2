# 🎯 6-Axis Closed-Loop System - Migration Summary

## ✅ What Changed

### System Configuration
- **Previous**: 8 axes (XYZABCDE / Joints 0-7)
- **Current**: **6 axes (XYZABC / Joints 0-5)**
- **Reason**: More practical for most CNC/Robot applications, frees up GPIO pins

---

## 📁 Updated Files

### 1. **Core Firmware** ✅
- `src/remora-core/configuration.h`
  - Changed `Config::joints` from 8 to 6
  - Comment updated: "Number of joints (XYZABC - 6-axis)"

### 2. **Configuration Files** ✅
- **NEW**: `config_6axis_closed_loop.json`
  - Complete 6-axis configuration
  - All modules per axis: Stepgen, Encoder, Position Monitor, Alarm, Endstop
  - Organized with visual separators per joint
  - Clear comments for each module

### 3. **Pin Reference** ✅
- **REPLACED**: `PIN_REFERENCE.md` (was 8-axis, now 6-axis)
  - Complete pin mapping for 6 axes
  - Visual ASCII diagrams per axis
  - Quick reference tables (Linear XYZ + Rotary ABC)
  - Reserved pins section (Ethernet RMII)
  - Available pins for expansion
  - Alarm wiring details (differential A+/A-)
  - Data bit mapping
  - Testing checklist

### 4. **Main README** ✅
- `README.md`
  - Updated closed-loop features section
  - Now shows "6-Axis System (XYZABC)"
  - Updated documentation links
  - Added supported configurations table
  - Points to PIN_REFERENCE.md as main reference

### 5. **Alarm Documentation** ✅
- `ALARM_QUICK_REFERENCE.md`
  - Updated from 8 to 6 axes
  - Pin mapping table shows joints 0-5
  - Bits 16-21 (was 16-23)

---

## 🔌 Pin Allocation Summary

### 6-Axis System Uses:
```
Step/Dir/Enable: 18 pins (6 axes × 3 pins)
Encoder A/B:     12 pins (6 axes × 2 pins)
Alarm A-:         6 pins (6 axes × 1 pin, A+ shared)
Endstops:         6 pins (6 axes × 1 pin)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
TOTAL:           42 GPIO pins
```

### Freed Pins (vs 8-axis):
```
Previously used (8-axis): 56 pins
Now used (6-axis):        42 pins
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Freed up:                 14 pins! ✅
```

### Available for Expansion:
- Spindle PWM control
- Coolant/Mist outputs
- Probe input
- MPG/Handwheel encoder
- Additional endstops (home switches)
- Relay outputs
- Extra I/O

---

## 📊 Data Structure Changes

### txData.inputs Bit Mapping (Now 6-axis):
```
Bits 0-5:   Endstops (XYZABC)
Bits 6-7:   Reserved
Bits 8-13:  Position Monitor Faults (XYZABC)
Bits 14-15: Reserved
Bits 16-21: Motor Alarms (XYZABC)
Bits 22-31: Reserved
```

### Arrays (Config::joints = 6):
```cpp
rxData.jointFreqCmd[6]    // Commands from LinuxCNC
txData.jointFeedback[6]   // Encoder feedback
txData.jointStepCount[6]  // Stepgen count
txData.processVariable[6] // Position errors
```

---

## 🎯 Supported Machine Types

### ✅ Fully Supported (6-axis firmware):

1. **3-Axis CNC Mill/Router**
   - Use: XYZ (joints 0-2)
   - Disable: ABC in LinuxCNC INI

2. **4-Axis Mill + Rotary**
   - Use: XYZA (joints 0-3)
   - Disable: BC in LinuxCNC INI

3. **5-Axis Mill (Nutating Head)**
   - Use: XYZAB (joints 0-4)
   - Disable: C in LinuxCNC INI

4. **5-Axis Mill (Trunnion Table)**
   - Use: XYZAC (joints 0-2, 3, 5)
   - Disable: B (joint 4) in LinuxCNC INI

5. **6-Axis Robot Arm**
   - Use: All XYZABC (joints 0-5)
   - Full kinematics

6. **Lathe with C-Axis**
   - Use: XZC (joints 0, 2, 5)
   - Disable: YAB in LinuxCNC INI

---

## 🔧 How to Use

### For 3-Axis Machine:
```ini
# LinuxCNC INI file
[KINS]
KINEMATICS = trivkins coordinates=XYZ

[TRAJ]
COORDINATES = XYZ
JOINTS = 3

# Only configure [JOINT_0], [JOINT_1], [JOINT_2]
```

### For 4-Axis Machine:
```ini
[KINS]
KINEMATICS = trivkins coordinates=XYZA

[TRAJ]
COORDINATES = XYZA
JOINTS = 4

# Configure [JOINT_0] through [JOINT_3]
```

### For Full 6-Axis:
```ini
[KINS]
KINEMATICS = trivkins coordinates=XYZABC
# or use gantrykins, genserkins, etc. for complex kinematics

[TRAJ]
COORDINATES = XYZABC
JOINTS = 6

# Configure all [JOINT_0] through [JOINT_5]
```

---

## 📝 Migration Checklist

If you were using the old 8-axis version:

- [ ] **Firmware**: Recompile with updated `configuration.h` (Config::joints = 6)
- [ ] **JSON Config**: Use new `config_6axis_closed_loop.json` as template
- [ ] **Wiring**: Remove joints 6-7 (D/E axes) if they existed
- [ ] **LinuxCNC INI**: Update `JOINTS = 6` (or less if not using all)
- [ ] **HAL File**: Remove references to joints 6-7
- [ ] **Pin Reference**: Use new `PIN_REFERENCE.md` for correct pins

---

## 🆕 New Features (vs Old 8-Axis)

1. ✅ **Better organized PIN_REFERENCE.md**
   - Visual ASCII diagrams per axis
   - Quick reference tables
   - Clear separation of linear/rotary axes

2. ✅ **Complete 6-axis config example**
   - All modules configured
   - Visual separators per joint
   - Detailed comments

3. ✅ **More realistic system**
   - 6 axes covers 99% of CNC/robot applications
   - Frees up pins for extras (spindle, probe, etc.)
   - Easier to understand and maintain

4. ✅ **Updated all documentation**
   - README points to PIN_REFERENCE as main guide
   - Alarm docs updated for 6 axes
   - Consistent throughout

---

## 📞 Next Steps

1. **Read PIN_REFERENCE.md** - Main reference for all pin assignments
2. **Copy config_6axis_closed_loop.json** - Use as template for your machine
3. **Wire your servos** - Follow alarm wiring guide (A+/A- differential)
4. **Configure LinuxCNC** - Set JOINTS to how many axes you're using
5. **Test per axis** - Use testing checklist in PIN_REFERENCE.md

---

## 🔗 Documentation Links

| Document | Purpose |
|----------|---------|
| **PIN_REFERENCE.md** | 🌟 **START HERE** - Complete pin mapping |
| config_6axis_closed_loop.json | Full 6-axis configuration template |
| README.md | Project overview and features |
| ETHERNET_CLOSED_LOOP_GUIDE.md | Closed-loop system setup |
| ALARM_QUICK_REFERENCE.md | Alarm wiring cheat sheet |
| ALARM_DIFFERENTIAL_WIRING.md | Detailed alarm wiring diagrams |

---

**Created:** October 26, 2025  
**Migration:** 8-axis → 6-axis  
**Board:** XCore407i (STM32F407IGT6)  
**System:** Closed-Loop Ethernet Remora
