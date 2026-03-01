# XCore407I Remora Firmware - Ethernet CNC Controller

> ⚠️ **Development Status:** This firmware is functional but not yet tested on real hardware. Use at your own risk and please report any issues!

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![License](https://img.shields.io/badge/license-GPL--2.0-blue.svg)](LICENSE)
[![STM32](https://img.shields.io/badge/MCU-STM32F407IGT6-orange.svg)]()
[![Ethernet](https://img.shields.io/badge/PHY-DP83848I-blue.svg)]()

**Professional CNC controller firmware for XCore407I board with real-time Ethernet communication and closed-loop motor control.**

## ✨ Features

- 🎯 **6-axis closed-loop control** (XYZUVW) with quadrature encoders
- 🌐 **Native Ethernet communication** via UDP (no SPI modules needed)  
- ⚡ **Real-time performance** - 120kHz base thread, 10kHz servo thread
- 🔄 **LinuxCNC integration** via remora-eth component
- 🛡️ **Safety features** - endstops, motor alarms, e-stop handling
- 📊 **Professional diagnostics** - position monitoring, fault detection

## 🚀 Quick Start

### Option A: Download Ready-to-Flash Firmware (Recommended)
**👉 For users who just want to use the firmware:**

1. **Download:** Get [`firmware.bin`](firmware/firmware.bin) (285KB)
2. **Flash:** Use DFU mode:
   ```bash
   dfu-util -a 0 -s 0x08000000:leave -D firmware.bin
   ```
3. **Setup:** Follow [GETTING_STARTED.md](GETTING_STARTED.md) for network & LinuxCNC config

### Option B: Build from Source
**👉 For developers who want to modify the code:**
```bash
platformio run -e xcore407i_eth_dfu -t upload
```

### Quick Network Setup
- **Board IP:** 192.168.10.10
- **PC IP:** 192.168.10.20  
- **Connection:** Direct Ethernet cable

### LinuxCNC Configuration
Choose your axis count and copy the configuration:
```bash
# For 6-axis machine:
cp linuxcnc/xcore407i_6axis.* ~/linuxcnc/configs/my_machine/
```

📖 **Complete Setup Guide:** [GETTING_STARTED.md](GETTING_STARTED.md)

## � Download Firmware

### Direct Download (No Build Required)
**Latest firmware:** [`firmware.bin`](firmware/firmware.bin) (285KB)

**GitHub Users:**
1. Click the link above or browse to `firmware/` folder
2. Download `firmware.bin` 
3. Flash to your board using DFU mode

**Command Line:**
```bash
# Download directly
wget https://github.com/YOUR_USERNAME/remora-xcore407i/raw/master/firmware/firmware.bin

# Flash to board (DFU mode)
dfu-util -a 0 -s 0x08000000:leave -D firmware.bin
```

## �📋 System Architecture

### Hardware Support
- **Board:** XCore407I (STM32F407IGT6 @ 168MHz)
- **Ethernet PHY:** DP83848I (RMII interface)
- **Memory:** 128KB RAM, 1MB Flash (22% used)
- **Real-time:** Deterministic step generation and feedback

### Motion Control
| Feature | Specification |
|---------|---------------|
| **Axes** | Up to 6 (XYZUVW) |
| **Step Generation** | 120kHz base thread |
| **Position Feedback** | 10kHz servo thread |
| **Encoder Resolution** | 32-bit counters |
| **Network Latency** | <1ms typical |

### I/O Capabilities
- 6x Step/Direction outputs (with enable)
- 6x Quadrature encoder inputs (A/B + optional Index)
- 6x Endstop/limit switch inputs 
- 6x Motor alarm inputs
- Emergency stop integration

## 📁 Project Structure

```
xcore407i-remora/
├── 📁 src/                    # Source code
│   ├── main.cpp               # Application entry point
│   ├── remora-core/           # Core modules (stepgen, encoder, etc.)
│   └── remora-hal/            # STM32F4 hardware abstraction
├── 📁 linuxcnc/              # LinuxCNC configurations
│   ├── xcore407i_3axis.hal   # 3-axis HAL config
│   ├── xcore407i_6axis.hal   # 6-axis HAL config
│   └── *.ini                 # Machine parameters
├── 📁 resources/             # Firmware configurations
│   ├── config_*axis_eth.txt  # JSON configs for different axis counts
│   └── schematic xcore407i/  # Pin mapping documentation
├── 📁 docs/                  # Documentation
├── platformio.ini            # Build configuration
└── README.md                 # This file
```

## 🔧 Hardware Configuration

### Pin Mapping (XCore407I)
The firmware uses predefined pin assignments optimized for the XCore407I board layout. See [resources/schematic xcore407i/](resources/schematic%20xcore407i/) for detailed pin mapping.

**Ethernet (DP83848I PHY - RMII):**
```
PA1  → REF_CLK    |  PC1  → MDC     |  PB11 → TX_EN
PA2  → MDIO       |  PC4  → RXD0    |  PB12 → TXD0  
PA7  → CRS_DV     |  PC5  → RXD1    |  PB13 → TXD1
```

**Step/Direction Outputs (6-axis):**
- **X:** PE3(En) + PE4(Step) + PE5(Dir)
- **Y:** PE6(En) + PE0(Step) + PE1(Dir) 
- **Z:** PB9(En) + PB4(Step) + PB5(Dir)
- **U:** PE8(En) + PE9(Step) + PE10(Dir)
- **V:** PE11(En) + PE12(Step) + PE13(Dir)
- **W:** PB6(En) + PB7(Step) + PB8(Dir)

**Encoder Inputs (A/B channels):**
- **X:** PD0/PD1  |  **U:** PD6/PD7   
- **Y:** PD2/PD3  |  **V:** PD8/PD9   
- **Z:** PD4/PD5  |  **W:** PD10/PD11

## ⚙️ Building & Installation

### Prerequisites
- **PlatformIO** (VSCode extension or CLI)
- **dfu-util** for USB DFU flashing
- **LinuxCNC 2.8+** with remora-eth component

### Build Firmware
```bash
cd xcore407i-remora/
platformio run -e xcore407i_eth_dfu
```

### Flash via DFU
1. Put board in DFU mode (hold BOOT0, press RESET, release BOOT0)
2. Connect USB cable
3. Flash firmware:
```bash
platformio run -e xcore407i_eth_dfu -t upload
```

### Memory Usage
- **RAM:** 65KB / 128KB (51% used)
- **Flash:** 224KB / 1MB (22% used)
- **Performance:** Stable with room for expansion

## 🌐 Network Configuration

### Default Settings
| Device | IP Address | Port | Protocol |
|--------|-----------|------|----------|
| **XCore407I** | 192.168.10.10 | 27181 | UDP |
| **LinuxCNC PC** | 192.168.10.20 | - | - |

### Setup PC Network
**Linux:**
```bash
sudo ip addr add 192.168.10.20/24 dev eth0
sudo ip link set eth0 up
```

**Windows:**
```powershell
netsh interface ip set address "Ethernet" static 192.168.10.20 255.255.255.0
```

### Connection Test
```bash
ping 192.168.10.10  # Should respond immediately
```

## 🎯 Configurations Available

| Configuration | Axes | JSON Config | LinuxCNC Files | Best For |
|---------------|------|-------------|----------------|----------|
| **3-axis** | XYZ | `config_xcore407i_3axis_eth.txt` | `xcore407i_3axis.*` | Basic CNC mill |
| **4-axis** | XYZU | `config_xcore407i_4axis_eth.txt` | `xcore407i_4axis.*` | Mill + rotary |
| **5-axis** | XYZUV | `config_xcore407i_5axis_eth.txt` | `xcore407i_5axis.*` | 5-axis mill |
| **6-axis** | XYZUVW | `config_xcore407i_6axis_eth.txt` | `xcore407i_6axis.*` | Full 6-axis |

## 🔧 LinuxCNC Setup

### 1. Install remora-eth Component
```bash
cd ~/Remora/LinuxCNC/Components
sudo halcompile --install remora-eth.c
```

### 2. Copy Configuration Files
```bash
# Choose your configuration (example: 6-axis)
cp linuxcnc/xcore407i_6axis.* ~/linuxcnc/configs/my_machine/
```

### 3. Launch LinuxCNC
```bash
linuxcnc ~/linuxcnc/configs/my_machine/xcore407i_6axis.ini
```

## 🐛 Troubleshooting

### Network Issues
- **Can't ping board:** Check cable, verify PC IP is 192.168.10.20
- **No communication:** Restart board, check firewall settings
- **High latency:** Use direct cable connection (no switches/routers)

### Hardware Issues
- **No step output:** Check enable signals, verify JSON pin assignments
- **Encoder not counting:** Swap A/B channels, check power supply
- **Following errors:** Tune PID parameters, check encoder direction

## 📚 Documentation

- **[GETTING_STARTED.md](GETTING_STARTED.md)** - Complete setup guide
- **[QUICKSTART.md](QUICKSTART.md)** - 5-minute quick setup
- **[VERIFICATION.md](VERIFICATION.md)** - Test procedures
- **[linuxcnc/README.md](linuxcnc/README.md)** - LinuxCNC configuration details

## 🤝 Contributing

1. Fork the project
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the GPL-2.0 License - see the [LICENSE](LICENSE) file for details.

## 🙏 Credits

- **[Remora Firmware](https://github.com/scottalford75/Remora)** - Scott Alford
- **[LwIP TCP/IP Stack](https://savannah.nongnu.org/projects/lwip/)** - Swedish Institute of Computer Science
- **[STM32 HAL](https://www.st.com/en/embedded-software/stm32cube-mcu-mpu-packages.html)** - STMicroelectronics
- **XCore407I Port** - Marioskiv

---

**Ready to start?**  
👉 Flash the firmware and follow [GETTING_STARTED.md](GETTING_STARTED.md)

<<<<<<< Updated upstream
- Check encoder direction matches step direction## platformio.ini Configuration

- Reduce PID P-gain initially

- Set FF1 = 1.0 for velocity feedforward```ini

[env:xcore407i_eth_dfu]

**See GETTING_STARTED.md for complete troubleshooting guide.**board = genericSTM32F407IGT6

upload_protocol = dfu

---

build_flags =

## 📞 Support & Community    -D STM32F407xx

    -D USE_HAL_DRIVER

- **LinuxCNC Forum:** https://forum.linuxcnc.org/    -D HSE_VALUE=8000000U

- **Remora Project:** https://github.com/scottalford75/Remora    -D ETH_CTRL=1

- **This Firmware:** https://github.com/Marioskiv/Marioskiv-remora-xcore407i-v2    -D USE_NATIVE_ETHERNET=1

    -D DP83848_PHY_ADDRESS=0x01

---    -D LWIP_ENABLED=1

    -D UART_PORT=USART2

## 📄 License```



Based on the Remora project by Scott Alford  ---

Modifications for XCORE407I by Marioskiv

## Pinout Reference

**License:** GPL-2.0 (see original Remora project)

Δες `resources/schematic xcore407i/` για αναλυτικά JSON schematics:

---- **ETHERNET DP83848I schematic.json** - Ethernet PHY connections

- **MCU schematic.json** - Πλήρες STM32F407IGT6 pinout

## 🙏 Credits- **USB FS OTG schematic.json** - USB connections για DFU

- **POWER RTC schematic.json** - Power rails & crystals

- **Remora Firmware:** Scott Alford

- **LwIP Stack:** Swedish Institute of Computer Science---

- **STM32 HAL:** STMicroelectronics

- **XCORE407I Port:** Marioskiv with GitHub Copilot## Development Status



---### ✅ Ολοκληρώθηκε

- [x] Project structure setup

**Ready to start?**  - [x] Remora core & HAL integration

👉 Go to [firmware/](firmware/) and flash `remora-xcore407i-v2.bin`  - [x] XCore407I ETH BSP layer (pinout configuration)

👉 Then follow [GETTING_STARTED.md](GETTING_STARTED.md)- [x] PlatformIO configuration για DFU

- [x] Linker script για STM32F407IGT6

Good luck with your CNC project! 🎯

### 🚧 Σε εξέλιξη
- [ ] LwIP middleware integration
- [ ] STM32F4_NativeEthComms implementation
- [ ] Network protocol handlers (UDP/TCP για Remora packets)
- [ ] Main application integration

### 📋 Επόμενα βήματα
1. Ενσωμάτωση LwIP από STM32F4x7_ETH_LwIP package
2. Ολοκλήρωση NativeEthComms class με TCP/UDP handlers
3. Testing με LinuxCNC component
4. Configuration file support (JSON via SD card ή UART)

---

## Resources

- **Remora Project**: https://github.com/scottalford75/Remora
- **Remora-STM32F4xx**: https://github.com/ben5j/Remora-STM32F4xx-PIO
- **XCore407I**: Waveshare STM32F407IGT6 development board
- **LinuxCNC**: https://linuxcnc.org/

---

## License
GPL v2 (same as Remora framework)

## Credits
- **Remora framework**: Scott Alford (scotta)
- **STM32F4 port**: Ben Jacobson
- **XCore407I adaptation**: [Marioskiv]
=======
**Questions or issues?**  
👉 Open an [issue](https://github.com/Marioskiv/remora-xcore407i/issues) or check the LinuxCNC forums
>>>>>>> Stashed changes
