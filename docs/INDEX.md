# Documentation Index

## 🚀 Getting Started

### Quick Start Guides
1. **[QUICKSTART.md](QUICKSTART.md)** ⭐ **START HERE FIRST**
   - Flash firmware (DFU mode)
   - PC network setup (Windows/Linux)
   - Hardware connection (direct Ethernet cable)
   - Ping test & verification
   - 5 minutes to working system!

2. **[NETWORK_SETUP.md](NETWORK_SETUP.md)** 📡 **COMPLETE NETWORK REFERENCE**
   - Detailed PC network configuration (3 methods for Windows, 3 for Linux)
   - Cable types (Auto-MDIX, crossover)
   - 5 testing procedures with expected outputs
   - Comprehensive troubleshooting (ping, ARP, firewall, PHY LEDs)
   - Wireshark packet analysis
   - Alternative router/switch setup

---

## 🔧 Hardware Reference

### Pin Mappings
- **[PIN_REFERENCE.md](PIN_REFERENCE.md)** - Complete pin mapping με visual guides
  - Ethernet PHY (DP83848I RMII)
  - Stepgen outputs (step/dir)
  - Encoder inputs (A/B/Z)
  - Alarm inputs (differential A+/A-)
  - Endstop inputs
  - Digital I/O

- **[PIN_ASSIGNMENT.md](PIN_ASSIGNMENT.md)** - Detailed pin assignments
  - GPIO configurations
  - Alternate function mappings
  - Pull-up/pull-down settings

---

## 🎯 Closed-Loop System

### Setup Guides
- **[ETHERNET_CLOSED_LOOP_GUIDE.md](ETHERNET_CLOSED_LOOP_GUIDE.md)** - **MAIN CLOSED-LOOP GUIDE**
  - Position monitoring system
  - Encoder feedback integration
  - Following error detection
  - Alarm system integration
  - LinuxCNC HAL configuration

- **[CLOSED_LOOP_SETUP.md](CLOSED_LOOP_SETUP.md)** - Quick setup instructions
  - Module configuration
  - Threshold tuning
  - Testing procedures

---

## ⚠️ Motor Alarm System

### Differential Alarm Wiring
- **[ALARM_QUICK_REFERENCE.md](ALARM_QUICK_REFERENCE.md)** ⚡ **QUICK REFERENCE**
  - Pin assignments for 6 axes (X/Y/Z/A/B/C)
  - A+/A- wiring tables
  - Common +3.3V connection
  - Visual wiring diagram

- **[ALARM_DIFFERENTIAL_WIRING.md](ALARM_DIFFERENTIAL_WIRING.md)** - **DETAILED DIAGRAMS**
  - Schematic diagrams
  - Servo driver connections (Leadshine, Delta, Yaskawa)
  - Photoelectric isolation circuits
  - Pull-up resistor calculations

- **[ALARM_WIRING_GUIDE.md](ALARM_WIRING_GUIDE.md)** - **TROUBLESHOOTING GUIDE**
  - Common problems & solutions
  - Multimeter testing procedures
  - Logic level verification
  - Noise immunity best practices

---

## 📋 Configuration Examples

All configuration files are in JSON format and can be loaded via SD card or UART.

### Available Configurations

- **[examples/config_6axis_closed_loop.json](examples/config_6axis_closed_loop.json)** ⭐ **RECOMMENDED**
  - 6-axis system (XYZABC)
  - Stepgen + Encoder + Position Monitor + Alarm + Endstop per axis
  - Organized by axis with clear comments
  - Ready to use for most CNC applications

- **[examples/config_6axis_closed_loop.json](examples/config_6axis_closed_loop.json)**
  - 6-axis system (XYZABC)
  - Extended configuration for complex machines
  - Robot arms, multi-head systems

- **[examples/config_closed_loop_with_monitoring.json](examples/config_closed_loop_with_monitoring.json)**
  - Enhanced monitoring features
  - Additional debug outputs
  - Development/testing configuration

### Configuration File Structure

Each JSON config contains:
- **Thread definitions** (Base: 40kHz for stepgen/encoder, Servo: 1kHz for monitoring/comms)
- **Module instances** (Stepgen, Encoder, PositionMonitor, Alarm, Endstop, DigitalPin)
- **Pin assignments** (GPIO pins for each module)
- **Parameters** (step/dir timing, encoder PPR, error thresholds, debounce times)

---

## 📊 Documentation by Topic

### For First-Time Users
1. Read [QUICKSTART.md](QUICKSTART.md) (5 minutes)
2. Flash firmware
3. Setup network (follow prompts)
4. Test ping
5. Proceed to closed-loop setup

### For Network Issues
1. Check [NETWORK_SETUP.md](NETWORK_SETUP.md) troubleshooting section
2. Verify PC IP: `ipconfig` (Windows) or `ip addr` (Linux)
3. Test ping: `ping 192.168.10.10`
4. Check ARP: `arp -a | findstr "192.168.10.10"`
5. Check firewall rules (UDP port 27181)

### For Hardware Wiring
1. Start with [PIN_REFERENCE.md](PIN_REFERENCE.md) for overview
2. Use [ALARM_QUICK_REFERENCE.md](ALARM_QUICK_REFERENCE.md) for alarm pins
3. Check [ALARM_DIFFERENTIAL_WIRING.md](ALARM_DIFFERENTIAL_WIRING.md) for detailed schematics
4. Refer to [PIN_ASSIGNMENT.md](PIN_ASSIGNMENT.md) for GPIO details

### For Closed-Loop Setup
1. Read [ETHERNET_CLOSED_LOOP_GUIDE.md](ETHERNET_CLOSED_LOOP_GUIDE.md) (complete overview)
2. Choose configuration from [examples/](examples/) folder
3. Modify JSON for your hardware
4. Load via SD card or UART
5. Follow [CLOSED_LOOP_SETUP.md](CLOSED_LOOP_SETUP.md) for tuning

### For Troubleshooting Alarms
1. Check [ALARM_QUICK_REFERENCE.md](ALARM_QUICK_REFERENCE.md) for correct pin wiring
2. Follow [ALARM_WIRING_GUIDE.md](ALARM_WIRING_GUIDE.md) testing procedures
3. Verify A+/A- connections with multimeter
4. Check servo driver alarm output type (NPN/PNP/open-collector)
5. Test with HAL: `halcmd show pin remora.alarm.*`

---

## 🛠️ Development & Advanced

### Build & Flash
See main [README.md](../README.md) for:
- PlatformIO build instructions
- DFU flashing procedure
- ST-Link debugging setup

### Architecture
- Remora core framework (threads, modules, comms)
- STM32F4 HAL layer
- LwIP stack configuration
- UDP protocol implementation

### Firmware Modification
If modifying firmware:
- Module creation guide (in main README)
- Thread timing constraints
- Data structure layout (64-byte UDP packets)
- Build system (platformio.ini)

---

## 📝 Quick Reference Tables

### Network Configuration
| Device | IP | Subnet | Port | Protocol |
|--------|-----|--------|------|----------|
| STM32 | 192.168.10.10 | /24 | 27181 UDP | Server |
| LinuxCNC PC | 192.168.10.20 | /24 | 27181 UDP | Client |

### Supported Axis Configurations
| Config | Axes | Typical Use |
|--------|------|-------------|
| 3-Axis | XYZ (0-2) | Basic mill/router |
| 4-Axis | XYZA (0-3) | Rotary table |
| 5-Axis | XYZAB (0-4) | Tilting head |
| 6-Axis | XYZABC (0-5) | Full 6-axis / Robot |

### Module Types
| Module | Purpose | Thread | Frequency |
|--------|---------|--------|-----------|
| Stepgen | Step/dir output | Base | 40 kHz |
| Encoder | Quadrature input | Base | 40 kHz |
| PositionMonitor | Error detection | Servo | 1 kHz |
| Alarm | Fault input | Servo | 1 kHz |
| Endstop | Limit switches | Servo | 1 kHz |
| DigitalPin | GPIO | Servo | 1 kHz |

### Data Structures (UDP Packets)
| Packet | Size | Contains |
|--------|------|----------|
| rxData (PC→STM32) | 64 bytes | jointFreqCmd[6], setPoint[6], jointEnable, outputs |
| txData (STM32→PC) | 64 bytes | jointFeedback[6], processVariable[6], inputs, NVMPGinputs |

---

## 🔗 External Resources

- **Remora Project:** https://github.com/scottalford75/Remora
- **LinuxCNC:** https://linuxcnc.org/
- **STM32CubeF4:** https://www.st.com/en/embedded-software/stm32cubef4.html
- **LwIP:** https://savannah.nongnu.org/projects/lwip/

---

## 📧 Getting Help

If you need assistance:

1. **Network issues:** Include output from:
   ```powershell
   ipconfig
   ping 192.168.10.10
   arp -a
   ```

2. **Build errors:** Include full PlatformIO build output

3. **Hardware issues:** 
   - Check PHY LEDs on board
   - Verify cable connection
   - Test with multimeter (if applicable)

4. **Alarm troubleshooting:** Follow [ALARM_WIRING_GUIDE.md](ALARM_WIRING_GUIDE.md) test procedures first

---

**Last Updated:** October 26, 2025  
**Firmware Version:** UDP Ethernet Implementation (6-axis)  
**Compatible with:** LinuxCNC remora-eth component
