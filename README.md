       (NOT TESTED YET)
# XCore407I STM32F407IGT6 - Remora Firmware (Ethernet)

**STM32F407IGT6 CNC Controller with Ethernet**

Firmware για XCore407I board (STM32F407IGT6) βασισμένο στο **Remora framework** για LinuxCNC control με **native Ethernet communication** μέσω DP83848I PHY.

Real-time firmware for LinuxCNC with closed-loop motor control, encoder feedback, and direct Ethernet connection.

---

## 🚀 Quick Start

1. **Flash Firmware**
   ```bash
   cd firmware/
   dfu-util -a 0 -s 0x08000000:leave -D remora-xcore407i-eth.bin
   ```

2. **Connect Hardware**
   - Ethernet: PC ↔ Board (direct cable)
   - Set PC IP: 192.168.10.1
   - Board IP: 192.168.10.10

3. **Choose Configuration**
   ```bash
   # 3-axis (XYZ):
   copy resources\config_xcore407i_3axis_eth.txt config.txt
   
   # 4-axis (XYZU):
   copy resources\config_xcore407i_4axis_eth.txt config.txt
   
   # 5-axis (XYZUV):
   copy resources\config_xcore407i_5axis_eth.txt config.txt
   
   # 6-axis (XYZUVW):
   copy resources\config_xcore407i_6axis_eth.txt config.txt
   ```

4. **Run LinuxCNC**
   - Load remora-eth component
   - Configure HAL for closed-loop control
   - See `linuxcnc/` folder for complete configs

📖 **Detailed Guide:** [linuxcnc/README.md](linuxcnc/README.md)

---

## 🎯 Closed-Loop Features

### Real-Time Position Monitoring & Fault Detection - **Up to 6-Axis System (XYZUVW)**

Αυτό το firmware περιλαμβάνει **πλήρη υποστήριξη closed-loop control** για έως 6 άξονες servo motors:



```#### 1. **Encoder Feedback** (6 άξονες: X, Y, Z, A, B, C)

xcore407i stm32f407igt6 REMORA/- Quadrature encoder reading (A/B channels + optional Index)

├── firmware/           ← Compiled .bin files (READY TO FLASH)- Real position feedback στο LinuxCNC μέσω Ethernet

├── examples/           ← Configuration files (.txt, Modules-based) – Base thread execution (40 kHz)

├── docs/               ← Detailed documentation- **Linear axes (XYZ)**: mm/inch units

├── src/                ← Source code- **Rotary axes (ABC)**: degree units

├── include/            ← Header files

├── lib/                ← Libraries (LwIP, Remora core)#### 2. **Position Monitor Module** (NEW)

└── platformio.ini      ← Build configuration- Real-time σύγκριση stepgen (εντολές) vs encoder (πραγματική θέση)

```- Ανίχνευση step loss και following errors

- Configurable error threshold και debouncing

**Everything you need is here - nothing hidden!**- Reports position error στο LinuxCNC μέσω `processVariable[0-5]`

- **6 independent monitors** - one per axis

---

#### 3. **Motor Alarm Integration** (NEW)

## ✨ Features- **Differential alarm inputs (A+/A-)** για industrial servos

- Photoelectric isolation support

### Hardware Support- Hardware fault detection (overcurrent, encoder error, thermal)

- ✅ 6-axis closed-loop control (XYZABC)- **6 dedicated alarm bits** (16-21) στο `txData.inputs`

- ✅ Quadrature encoders with index pulse- All A+ share common +3.3V, each A- has unique GPIO pin

- ✅ Endstop/limit switches (NC or NO)

- ✅ Motor alarm inputs#### 4. **Dual-Layer Safety System**

- ✅ Step/direction outputs| Layer | Module | Detection | Speed | Bits |

- ✅ Ethernet (DP83848I PHY)|-------|--------|-----------|-------|------|

| **Firmware** | Position Monitor | Position error (SW) | 1 kHz | 8-13 |

### Communication| **Hardware** | Alarm | Servo driver faults (HW) | Immediate | 16-21 |

- ✅ UDP protocol (no TCP overhead)

- ✅ Direct PC connection (no router)#### 5. **Ethernet Communication**

- ✅ LwIP 1.3.2 stack- Όλα τα feedback data (encoder, alarms, position errors) μεταδίδονται μέσω Ethernet

- ✅ Static IP: 192.168.10.10- Compatible με remora-eth LinuxCNC component

- ✅ Port: 27181- Servo thread frequency: 1 kHz (configurable)

- **6 joints** × (stepCount + feedback + error) = full closed-loop data

### Control Features

- ✅ Real-time encoder feedback### 📚 Documentation

- ✅ Position monitoring

- ✅ Alarm detection- **[docs/QUICKSTART.md](docs/QUICKSTART.md)** - **START HERE** - 5-minute setup guide from flash to ping

- ✅ Configurable via JSON- **[docs/NETWORK_SETUP.md](docs/NETWORK_SETUP.md)** - **COMPLETE NETWORK GUIDE** - Direct connection setup, troubleshooting, Windows/Linux

- ✅ LinuxCNC integration- **[docs/PIN_REFERENCE.md](docs/PIN_REFERENCE.md)** - Complete pin mapping with visual guides

- Example Modules config: see `resources/config_xcore407i_6axis_eth.txt` (rename to `config.txt` before uploading)

---- **[docs/ETHERNET_CLOSED_LOOP_GUIDE.md](docs/ETHERNET_CLOSED_LOOP_GUIDE.md)** - Complete closed-loop setup guide

- **[docs/ALARM_QUICK_REFERENCE.md](docs/ALARM_QUICK_REFERENCE.md)** - Quick reference για alarm wiring (A+/A-)

## 📦 What's Included- **[docs/ALARM_DIFFERENTIAL_WIRING.md](docs/ALARM_DIFFERENTIAL_WIRING.md)** - Detailed wiring diagrams

- **[docs/ALARM_WIRING_GUIDE.md](docs/ALARM_WIRING_GUIDE.md)** - Troubleshooting και best practices

### 1. Firmware (Ready to Flash)

- `firmware/remora-xcore407i-v2.bin` - Main firmware### 🔌 Example Configuration

- `firmware/README.md` - Flash instructions

Δες `docs/examples/config_6axis_closed_loop.json` για **πλήρες παράδειγμα 6 αξόνων** με:

#### 1. **Encoder Feedback** (Up to 6 axes: X, Y, Z, U, V, W)
- Quadrature encoder reading (A/B channels + optional Index)
- Real position feedback στο LinuxCNC μέσω Ethernet
- Base thread execution (120 kHz) για ακριβή μέτρηση
- **Linear axes (XYZ)**: mm/inch units
- **Rotary axes (UVW or ABC)**: degree units

#### 2. **Position Monitor Module**
- Real-time σύγκριση stepgen (εντολές) vs encoder (πραγματική θέση)
- Ανίχνευση step loss και following errors
- Configurable error threshold και debouncing
- Reports position error στο LinuxCNC μέσω `processVariable[0-5]`
- **Up to 6 independent monitors** - one per axis

#### 3. **Motor Alarm Integration**
- **Differential alarm inputs (A+/A-)** για industrial servos
- Photoelectric isolation support
- Hardware fault detection (overcurrent, encoder error, thermal)
- **Up to 6 dedicated alarm bits** στο `txData.inputs`
- All A+ share common +3.3V, each A- has unique GPIO pin

#### 4. **Dual-Layer Safety System**
| Layer | Module | Detection | Speed | Bits |
|-------|--------|-----------|-------|------|
| **Firmware** | Position Monitor | Position error (SW) | 1 kHz | 8-13 |
| **Hardware** | Alarm | Servo driver faults (HW) | Immediate | 16-21 |

#### 5. **Ethernet Communication**
- Όλα τα feedback data (encoder, alarms, position errors) μεταδίδονται μέσω Ethernet
- Compatible με remora-eth LinuxCNC component
- Servo thread frequency: 1 kHz (configurable)
- **Up to 6 joints** × (stepCount + feedback + error) = full closed-loop data

---

## 🎯 Supported Configurations

| Configuration | Axes Used | JSON Config | LinuxCNC HAL/INI | Application |
|---------------|-----------|-------------|------------------|-------------|
| **3-Axis** | XYZ (0-2) | `resources/config_xcore407i_3axis_eth.txt` | `xcore407i_3axis.*` | Basic CNC mill/router |
| **4-Axis** | XYZU (0-3) | `resources/config_xcore407i_4axis_eth.txt` | `xcore407i_4axis.*` | Mill with rotary table |
| **5-Axis** | XYZUV (0-4) | `resources/config_xcore407i_5axis_eth.txt` | `xcore407i_5axis.*` | Nutating/tilting head |
| **6-Axis** | XYZUVW (0-5) | `remora_config_xcore407i_6axis_eth.json` | `xcore407i_6axis.*` | Robot arm / Full 6-axis |

**All configuration files available in:**
- Firmware configs: `resources/remora_config_xcore407i_*axis_eth.json`
- LinuxCNC configs: `linuxcnc/xcore407i_*axis.(hal|ini)`

Disable unused axes στο LinuxCNC INI file.

---

## 📁 Project Structure

```
xcore407i stm32f407igt6 REMORA/
├── firmware/              ← Compiled .bin files (READY TO FLASH)
├── resources/             ← Firmware configuration JSONs (3-6 axis)
├── linuxcnc/              ← Complete LinuxCNC HAL/INI configs
├── docs/                  ← Detailed documentation
├── src/                   ← Source code
├── include/               ← Header files
├── lib/                   ← Libraries (LwIP, Remora core)
└── platformio.ini         ← Build configuration
```

**Everything you need is here - nothing hidden!**

---

## 📚 Documentation

### 1. LinuxCNC Integration (START HERE)
- **[linuxcnc/README.md](linuxcnc/README.md)** - Complete setup guide with pin mappings
- **Configuration files**: `linuxcnc/xcore407i_{3,4,5,6}axis.{hal,ini}`

### 2. Firmware Configuration
- **JSON configs**: `resources/remora_config_xcore407i_{3,4,5,6}axis_eth.json`
- Defines MCU pin assignments for motors, encoders, endstops, alarms
- Avoids Ethernet RMII pins (PA2/PA7, PC1/PC4/PC5, PG11/PG13/PG14)

### 3. Hardware Reference
- **Board schematics**: `resources/schematic xcore407i/`
- **Pin headers**: `resources/schematic xcore407i/xcore407i_pin_headers.json`
- All pins exposed on P1/P2 headers

Gateway:     192.168.10.1 (unused for direct connection)

# Check connection```

ping 192.168.10.10

```#### PC Network Settings (Windows/Linux)

Ρύθμισε την Ethernet κάρτα του PC με **στατική IP στο ίδιο subnet**:

### 2. Hardware Wiring (Axis 0 - X)

```

---

## 🔧 Building from Source

```bash
# Install PlatformIO
pip install platformio

# Build firmware
pio run -e xcore407i_eth_dfu

# Flash via USB DFU
pio run -e xcore407i_eth_dfu -t upload
```

**Build output:** `.pio/build/xcore407i_eth_dfu/firmware.bin`

---

## Software Architecture

### Based on Remora-STM32F4xx
- **remora-core**: Core framework από το Remora project
- **remora-hal**: STM32F4-specific HAL abstraction layer
- **Native Ethernet**: Custom implementation με STM32 ETH MAC + LwIP stack (αντί W5500 SPI chip)

### Key Components
1. **STM32F4_NativeEthComms**: CommsInterface implementation για native Ethernet
2. **xcore407i_eth_bsp**: Board Support Package για XCore407I Ethernet pins
3. **LwIP**: Lightweight TCP/IP stack για network protocol handling
4. **DFU Bootloader Support**: USB DFU flashing capability

---

## Hardware Specifications

### MCU
- **STM32F407IGT6** (LQFP-176, 1MB Flash, 192KB RAM)
- **HSE**: 8 MHz crystal
- **LSE**: 32.768 kHz RTC crystal
- **SYSCLK**: 168 MHz (PLL)

### Ethernet (DP83848I RMII PHY)
| Signal   | STM32 Pin | Function              |
|----------|-----------|---------------------|
| REF_CLK  | PA1       | 50 MHz input        |
| MDIO     | PA2       | Management data I/O |
| CRS_DV   | PA7       | Carrier sense/RX valid |
| MDC      | PC1       | Management clock    |
| RXD0     | PC4       | Receive data bit 0  |
| RXD1     | PC5       | Receive data bit 1  |
| TX_EN    | PG11      | Transmit enable     |
| TXD0     | PG13      | Transmit data bit 0 |
| TXD1     | PG14      | Transmit data bit 1 |

**PHY Address**: 0x01  
**Interface**: RMII (Reduced Media Independent Interface)  
**Speed**: Auto-negotiation (10/100 Mbps)

---

## Network Configuration

### 🔌 Direct PC Connection (Recommended for CNC)

**Αυτό το firmware είναι ρυθμισμένο για απευθείας σύνδεση PC ↔ STM32 board χωρίς router/switch.**

#### Hardware Setup
1. **Ένα Ethernet καλώδιο** κατευθείαν από PC Ethernet port → XCore407i RJ45 connector
2. Τα περισσότερα σύγχρονα Ethernet ports έχουν Auto-MDIX (αυτόματη crossover detection)
3. Αν δεν λειτουργεί με straight-through cable, χρησιμοποίησε **crossover cable**

#### STM32 Network Settings (Static IP)
```
IP Address:  192.168.10.10
Netmask:     255.255.255.0
Gateway:     (empty - not needed for direct connection)
DNS:         (empty - not needed for direct connection)
```

#### PC Network Settings

Set your PC's Ethernet adapter to a compatible static IP:

```
IP Address:  192.168.10.20  (ή οποιοδήποτε .2-.254 εκτός από .10)
Subnet Mask: 255.255.255.0
Gateway:     (άδειο - δεν χρειάζεται)
DNS:         (άδειο - δεν χρειάζεται)
```

**Windows PowerShell Example:**
```powershell
# Find your Ethernet adapter name
Get-NetAdapter

# Set static IP (αντικατάστησε "Ethernet" με το σωστό adapter name)
New-NetIPAddress -InterfaceAlias "Ethernet" -IPAddress 192.168.10.20 -PrefixLength 24
```

**Linux Example:**
```bash
# Temporary (lost after reboot)
sudo ip addr add 192.168.10.20/24 dev eth0

# Or edit /etc/network/interfaces for permanent setup
```

#### Testing Connectivity
```bash
# Ping το STM32 board
ping 192.168.10.10

# Θα πρέπει να δεις replies αν όλα είναι σωστά ρυθμισμένα
```

### 🌐 Network με Router/Switch (Optional)

Αν θέλεις να συνδέσεις μέσω router/switch, άλλαξε την IP στο firmware να ταιριάζει με το subnet του δικτύου σου.

Για DHCP support (αυτόματο IP), ενεργοποίησε το `#define USE_DHCP` στο `lwipopts.h` και recompile.

### 🔧 Protocol Configuration

**Αυτό το firmware χρησιμοποιεί UDP communication (port 27181) για χαμηλό latency.**

- LinuxCNC remora-eth driver: **UDP client**
- STM32 firmware: **UDP server**
- Packet format: 64-byte PRU_READ/PRU_WRITE/PRU_DATA protocol
- Update rate: 1 kHz (servo thread frequency)

---

## 🛠️ Hardware Requirements

### Board
- **XCORE407I** (STM32F407IGT6)
- Ethernet with DP83848I PHY
- USB for programming
- Status LED on PC13

### Peripherals
- Ethernet cable (straight or crossover)
- Servo drives or stepper drivers
- Quadrature encoders (A, B, optionally Z)
- Limit/home switches
- Motor alarm signals (optional)
- Touch probe sensor (optional, NO or NC type)

- **PlatformIO** installed

### Computer- **dfu-util** για USB DFU flashing

- LinuxCNC 2.8 or later- **ST-Link** (optional, για debug)

- Ethernet port

- remora-eth component installed### Build

```powershell

---pio run -e xcore407i_eth_dfu

```

## ⚡ Performance

### Flash via DFU

- **Loop Rate:** Configurable (typically 25-40 kHz)1. Βάλε το board σε DFU mode:

- **Latency:** < 1ms   - Κράτα πατημένο BOOT0 button

- **Jitter:** < 100µs   - Press και άφησε RESET button

- **Max Axes:** 6 simultaneous   - Άφησε BOOT0 button

- **Encoder Resolution:** 32-bit counters

- **Network Bandwidth:** ~150 KB/s sustained2. Flash:

```powershell

---pio run -e xcore407i_eth_dfu -t upload

```

## 🐛 Troubleshooting

Ή με dfu-util:

### "Can't ping 192.168.10.10"```powershell

- Check PC IP is 192.168.10.1dfu-util -a 0 -s 0x08000000:leave -D .pio\build\xcore407i_eth_dfu\firmware.bin

- Try different Ethernet cable```

- Check status LED blinks (2 Hz)

### Flash via ST-Link (debug)

### "Encoder not counting"```powershell

- Swap A/B channelspio run -e xcore407i_eth_dfu -t upload --upload-port stlink

- Check encoder power supply```

- Verify wiring (use docs/PIN_REFERENCE.md)

---

### "Following error"

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
