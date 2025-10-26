# XCore407I Firmware - Build Status & Next Steps

## ✅ Completed Tasks

### 1. Project Structure
- ✅ Remora-core framework integrated
- ✅ Remora-hal STM32F4 HAL layer
- ✅ Main application files copied from Remora-STM32F4xx

### 2. Ethernet Hardware Support
- ✅ XCore407I ETH BSP layer created (`xcore407i_eth_bsp.c/h`)
- ✅ DP83848I PHY pinout configured:
  - PA1: REF_CLK, PA2: MDIO, PA7: CRS_DV
  - PC1: MDC, PC4: RXD0, PC5: RXD1
  - PB11: TX_EN, PB12: TXD0, PB13: TXD1
  - PG13: PHY_RESET

### 3. Network Stack
- ✅ LwIP v1.3.2 copied to `lib/lwip`
- ✅ STM32F4x7 ETH driver copied to `lib/STM32F4x7_ETH_Driver`
- ✅ Network configuration header (`netconf.h`):
  - Static IP: 192.168.10.10
  - Gateway: 192.168.10.254
  - Subnet: 255.255.255.0/24
  - MAC: 00:80:E1:CC:07:01

### 4. Build Configuration
- ✅ platformio.ini configured for:
  - Board: STM32F407IGT6
  - Upload: DFU protocol
  - Build flags: Ethernet + LwIP enabled
  - Include paths for LwIP & ETH driver
- ✅ Linker script: STM32F407IGTx_FLASH.ld (1MB Flash)

---

## 🚧 In Progress

### STM32F4_NativeEthComms Implementation
Currently implementing the CommsInterface for native Ethernet:

**Files:**
- `src/remora-hal/STM32F4_NativeEthComms.h` (created)
- `src/remora-hal/STM32F4_NativeEthComms.cpp` (needs implementation)

**Required Components:**
1. ETH peripheral initialization via HAL
2. LwIP stack init & periodic tasks
3. TCP/UDP packet handlers for Remora protocol
4. Link status monitoring
5. DMA buffer management

---

## 📋 Next Steps (Priority Order)

### Immediate (Before Build)
1. **Complete STM32F4_NativeEthComms.cpp**
   - Implement constructor & init()
   - Add LwIP integration (lwip_init, netif setup)
   - Implement TCP server for Remora packets
   - Add UDP handler as backup

2. **Create ethernetif.c**
   - Low-level Ethernet interface for LwIP
   - HAL_ETH callbacks
   - Packet TX/RX functions

3. **Adapt main.cpp**
   - Replace W5500 reference with NativeEthComms
   - Add conditional compilation for ETH vs SPI

4. **Create lwipopts.h**
   - LwIP configuration tailored for Remora
   - Memory pool sizes
   - TCP/UDP settings

### Build & Test
5. **First Compilation Test**
   ```powershell
   pio run -e xcore407i_eth_dfu
   ```
   - Resolve compilation errors
   - Check memory usage (Flash/RAM)

6. **Hardware Preparation**
   - Connect XCore407I to Ethernet
   - Verify PHY LEDs behavior
   - Set BOOT0 for DFU mode

7. **Flash & Initial Test**
   ```powershell
   pio run -e xcore407i_eth_dfu -t upload
   ```
   - Monitor UART output (115200 baud)
   - Verify Ethernet init messages
   - Check link status

### Network Testing
8. **Basic Connectivity**
   - Ping test: `ping 192.168.10.10`
   - ARP table check
   - Wireshark capture

9. **Remora Protocol**
   - Test with LinuxCNC Remora component
   - UDP packet exchange
   - Real-time data streaming

---

## 🔧 Configuration Files Status

| File | Status | Purpose |
|------|--------|---------|
| `platformio.ini` | ✅ Complete | Build & upload config |
| `netconf.h` | ✅ Complete | IP/MAC configuration |
| `xcore407i_eth_bsp.c/h` | ✅ Complete | Hardware init |
| `STM32F4_NativeEthComms.h` | ✅ Complete | Interface definition |
| `STM32F4_NativeEthComms.cpp` | ⏳ TODO | Implementation |
| `ethernetif.c` | ⏳ TODO | LwIP low-level driver |
| `lwipopts.h` | ⏳ TODO | LwIP configuration |
| `main.cpp` | 🔄 Needs adaptation | App entry point |

---

## 📊 Memory Estimate

### Flash Usage (estimated)
- Remora core: ~200 KB
- HAL drivers: ~100 KB
- LwIP stack: ~150 KB
- Application: ~50 KB
- **Total**: ~500 KB / 1024 KB available ✅

### RAM Usage (estimated)
- LwIP buffers: ~40 KB
- Ethernet DMA: ~10 KB
- Stack/Heap: ~20 KB
- Application: ~10 KB
- **Total**: ~80 KB / 192 KB available ✅

---

## 🐛 Known Issues & Workarounds

### Issue 1: LwIP v1.3.2 is Old
- **Impact**: Modern LwIP is v2.x
- **Workaround**: ST package v1.3.2 is stable and tested with STM32F4x7
- **Future**: Consider upgrading to LwIP v2.1+ if issues arise

### Issue 2: DFU Bootloader Offset
- **Status**: Currently using no bootloader (0x08000000)
- **Future**: May need bootloader at 0x08000000, app at 0x08008000

### Issue 3: PHY Reset Pin (PG13)
- **Verify**: Schematic shows PG13, confirm on actual board
- **Alternative**: If different, update `xcore407i_eth_bsp.c`

---

## 📚 Reference Documents

1. **XCore407I Schematics**: `resources/schematic xcore407i/`
2. **Remora Protocol**: LinuxCNC Remora documentation
3. **DP83848I Datasheet**: TI website
4. **STM32F407 Reference Manual**: RM0090
5. **LwIP Documentation**: lwip.wikia.com

---

## 🎯 Success Criteria

- [ ] Project compiles without errors
- [ ] Firmware boots and initializes Ethernet
- [ ] PHY link LED indicates connection
- [ ] Board responds to ping at 192.168.10.10
- [ ] LinuxCNC Remora component establishes connection
- [ ] Real-time motion control data exchange works

---

**Current Phase**: Implementation (Step 1-4)  
**Estimated Time to First Build**: ~2-3 hours of coding  
**Estimated Time to Working System**: ~1 day including testing

---

Last Updated: [Current Session]
