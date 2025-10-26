# XCore407I STM32F407IGT6 - Remora Firmware (Ethernet)

## Περιγραφή Project
Firmware για XCore407I board (STM32F407IGT6) βασισμένο στο **Remora framework** για LinuxCNC control με **native Ethernet communication** μέσω DP83848I PHY.

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
| TX_EN    | PB11      | Transmit enable     |
| TXD0     | PB12      | Transmit data bit 0 |
| TXD1     | PB13      | Transmit data bit 1 |
| PHY_RST  | PG13      | PHY reset (active low) |

**PHY Address**: 0x01  
**Interface**: RMII (Reduced Media Independent Interface)  
**Speed**: Auto-negotiation (10/100 Mbps)

---

## 🎯 Closed-Loop Features (NEW!)

### Real-Time Position Monitoring & Fault Detection - **6-Axis System (XYZABC)**

Αυτό το firmware περιλαμβάνει **πλήρη υποστήριξη closed-loop control** για 6 άξονες servo motors:

#### 1. **Encoder Feedback** (6 άξονες: X, Y, Z, A, B, C)
- Quadrature encoder reading (A/B channels + optional Index)
- Real position feedback στο LinuxCNC μέσω Ethernet
- Base thread execution (40 kHz) για ακριβή μέτρηση
- **Linear axes (XYZ)**: mm/inch units
- **Rotary axes (ABC)**: degree units

#### 2. **Position Monitor Module** (NEW)
- Real-time σύγκριση stepgen (εντολές) vs encoder (πραγματική θέση)
- Ανίχνευση step loss και following errors
- Configurable error threshold και debouncing
- Reports position error στο LinuxCNC μέσω `processVariable[0-5]`
- **6 independent monitors** - one per axis

#### 3. **Motor Alarm Integration** (NEW)
- **Differential alarm inputs (A+/A-)** για industrial servos
- Photoelectric isolation support
- Hardware fault detection (overcurrent, encoder error, thermal)
- **6 dedicated alarm bits** (16-21) στο `txData.inputs`
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
- **6 joints** × (stepCount + feedback + error) = full closed-loop data

### 📚 Documentation

- **[docs/QUICKSTART.md](docs/QUICKSTART.md)** - **START HERE** - 5-minute setup guide from flash to ping
- **[docs/NETWORK_SETUP.md](docs/NETWORK_SETUP.md)** - **COMPLETE NETWORK GUIDE** - Direct connection setup, troubleshooting, Windows/Linux
- **[docs/PIN_REFERENCE.md](docs/PIN_REFERENCE.md)** - Complete pin mapping with visual guides
- **[docs/examples/config_6axis_closed_loop.json](docs/examples/config_6axis_closed_loop.json)** - Full 6-axis configuration example
- **[docs/ETHERNET_CLOSED_LOOP_GUIDE.md](docs/ETHERNET_CLOSED_LOOP_GUIDE.md)** - Complete closed-loop setup guide
- **[docs/ALARM_QUICK_REFERENCE.md](docs/ALARM_QUICK_REFERENCE.md)** - Quick reference για alarm wiring (A+/A-)
- **[docs/ALARM_DIFFERENTIAL_WIRING.md](docs/ALARM_DIFFERENTIAL_WIRING.md)** - Detailed wiring diagrams
- **[docs/ALARM_WIRING_GUIDE.md](docs/ALARM_WIRING_GUIDE.md)** - Troubleshooting και best practices

### 🔌 Example Configuration

Δες `docs/examples/config_6axis_closed_loop.json` για **πλήρες παράδειγμα 6 αξόνων** με:
- ✅ Stepgen (step/dir commands) × 6
- ✅ Encoder (position feedback) × 6
- ✅ Position Monitor (error detection) × 6
- ✅ Alarm (motor faults) × 6
- ✅ Endstop (limits) × 6
- ✅ Organized by axis with clear comments

### 🎯 Supported Configurations

| Configuration | Axes Used | Application |
|---------------|-----------|-------------|
| **3-Axis** | XYZ (0-2) | Basic CNC mill/router |
| **4-Axis** | XYZA (0-3) | Mill with rotary table |
| **5-Axis (Head)** | XYZAB (0-4) | Nutating/tilting head mill |
| **5-Axis (Table)** | XYZAC (0-2,3,5) | Trunnion table mill |
| **6-Axis** | XYZABC (0-5) | Robot arm / Full 6-axis |

Disable unused axes στο LinuxCNC INI file.

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
Gateway:     192.168.10.1 (unused for direct connection)
```

#### PC Network Settings (Windows/Linux)
Ρύθμισε την Ethernet κάρτα του PC με **στατική IP στο ίδιο subnet**:

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
```powershell
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

## Build & Flash

### Prerequisites
- **PlatformIO** installed
- **dfu-util** για USB DFU flashing
- **ST-Link** (optional, για debug)

### Build
```powershell
pio run -e xcore407i_eth_dfu
```

### Flash via DFU
1. Βάλε το board σε DFU mode:
   - Κράτα πατημένο BOOT0 button
   - Press και άφησε RESET button
   - Άφησε BOOT0 button

2. Flash:
```powershell
pio run -e xcore407i_eth_dfu -t upload
```

Ή με dfu-util:
```powershell
dfu-util -a 0 -s 0x08000000:leave -D .pio\build\xcore407i_eth_dfu\firmware.bin
```

### Flash via ST-Link (debug)
```powershell
pio run -e xcore407i_eth_dfu -t upload --upload-port stlink
```

---

## platformio.ini Configuration

```ini
[env:xcore407i_eth_dfu]
board = genericSTM32F407IGT6
upload_protocol = dfu

build_flags =
    -D STM32F407xx
    -D USE_HAL_DRIVER
    -D HSE_VALUE=8000000U
    -D ETH_CTRL=1
    -D USE_NATIVE_ETHERNET=1
    -D DP83848_PHY_ADDRESS=0x01
    -D LWIP_ENABLED=1
    -D UART_PORT=USART2
```

---

## Pinout Reference

Δες `resources/schematic xcore407i/` για αναλυτικά JSON schematics:
- **ETHERNET DP83848I schematic.json** - Ethernet PHY connections
- **MCU schematic.json** - Πλήρες STM32F407IGT6 pinout
- **USB FS OTG schematic.json** - USB connections για DFU
- **POWER RTC schematic.json** - Power rails & crystals

---

## Development Status

### ✅ Ολοκληρώθηκε
- [x] Project structure setup
- [x] Remora core & HAL integration
- [x] XCore407I ETH BSP layer (pinout configuration)
- [x] PlatformIO configuration για DFU
- [x] Linker script για STM32F407IGT6

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
- **XCore407I adaptation**: [Your name]
