# Changelog

All notable changes to this project will be documented in this file.

## [1.0.0] - 2026-03-01

### ✨ Features
- **6-axis closed-loop control** with encoder feedback
- **Native Ethernet communication** via DP83848I PHY (RMII)
- **UDP protocol** for real-time data exchange (port 27181)
- **LinuxCNC integration** via remora-eth component
- **Safety features** - endstops, motor alarms, e-stop
- **Real-time performance** - 120kHz base, 10kHz servo threads
- **Position monitoring** with following error detection
- **Professional documentation** and setup guides

### 🔧 Hardware Support
- **STM32F407IGT6** @ 168MHz (XCore407I board)
- **Quadrature encoders** (A/B + optional Index) for all 6 axes
- **Step/Direction outputs** with enable signals
- **Digital I/O** for endstops and alarm inputs
- **Direct Ethernet** connection (no router required)

### 📁 Configurations
- **3-axis (XYZ)** - Basic CNC mill setup
- **4-axis (XYZU)** - Mill with rotary table  
- **5-axis (XYZUV)** - 5-axis machining
- **6-axis (XYZUVW)** - Full 6-axis control

### 📦 Distribution
- **Pre-compiled firmware** (`firmware.bin`) ready to flash
- **Complete LinuxCNC configs** (HAL + INI files)
- **JSON configurations** for different axis counts
- **Detailed documentation** and troubleshooting guides

### 🔧 Build System
- **PlatformIO** build environment
- **STM32 HAL drivers** integration
- **LwIP TCP/IP stack** for networking
- **ArduinoJson** for configuration parsing
- **Memory optimized** - 22% flash, 51% RAM usage

### 🐛 Bug Fixes
- Fixed Ethernet initialization timing
- Corrected encoder direction handling
- Resolved UDP packet alignment issues  
- Fixed pin mapping inconsistencies

### 📖 Documentation
- Complete README with quick start guide
- Hardware pin mapping documentation
- LinuxCNC setup instructions
- Network configuration guide
- Troubleshooting and FAQ

---

## Development History

This firmware is based on the Remora project by Scott Alford, with extensive modifications for XCore407I hardware and Ethernet communication.

**Credits:**
- **Remora Framework:** Scott Alford
- **XCore407I Port:** Marioskiv  
- **LwIP Integration:** Based on STM32 examples
- **Hardware Abstraction:** STMicroelectronics HAL