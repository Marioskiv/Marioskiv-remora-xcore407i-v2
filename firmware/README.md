# Firmware Files

## Ready-to-Flash Binaries

### remora-xcore407i-v2.bin
**Main firmware file** - Flash this to your XCORE407I board

**Features:**
- 6-axis closed-loop control
- UDP Ethernet communication (192.168.10.10:27181)
- Encoder feedback (quadrature A/B + Index)
- Endstop/limit switch support
- Motor alarm monitoring
- Direct PC connection (no router needed)

---

## How to Flash

### Method 1: USB DFU (Recommended)

```bash
# 1. Put board in DFU mode:
#    - Press and hold BOOT0 button
#    - Press and release RESET button
#    - Release BOOT0 button

# 2. Flash firmware:
dfu-util -a 0 -s 0x08000000:leave -D remora-xcore407i-v2.bin
```

### Method 2: PlatformIO

```bash
# From project root:
pio run -e xcore407i_eth_dfu -t upload
```

### Method 3: ST-Link

Use STM32CubeProgrammer or st-flash to program `remora-xcore407i-v2.bin` to address `0x08000000`

---

## Files Included

| File | Purpose | Size |
|------|---------|------|
| `remora-xcore407i-v2.bin` | Flash firmware | ~150KB |
| `remora-xcore407i-v2.elf` | Debug symbols (optional) | ~3MB |

---

## After Flashing

1. **LED Check**: Status LED should blink at 2 Hz
2. **Network Test**: `ping 192.168.10.10`
3. **Configuration**: Use examples from `../examples/` folder
4. **Documentation**: See `../docs/` for detailed setup

---

## Firmware Version

**Build Date:** October 26, 2025  
**Version:** 2.0  
**Platform:** STM32F407IGT6  
**Protocol:** UDP (LwIP 1.3.2)  
**Max Axes:** 6 (XYZABC)

---

## Quick Start

```bash
# 1. Flash firmware (see above)
# 2. Connect Ethernet cable directly PC ↔ Board
# 3. Set PC IP: 192.168.10.1 / 255.255.255.0
# 4. Test connection: ping 192.168.10.10
# 5. Load LinuxCNC with remora-eth component
```

See **../GETTING_STARTED.md** for complete setup guide.
