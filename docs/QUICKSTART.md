# Quick Start Guide - XCore407I Remora Firmware

## ✅ Current Status

✅ **Project structure ready**  
✅ **Ethernet hardware layer complete**  
✅ **LwIP and drivers integrated**  
✅ **UDP communication implemented**  
✅ **Data structures synchronized with LinuxCNC driver**  
✅ **Build successful - firmware ready to flash**

---

## 🚀 Quick Setup (5 minutes)

### 1. Flash Firmware to STM32

#### Step 1a: Put the board in DFU mode
1. Press and hold the **BOOT0** button
2. Press and release the **RESET** button
3. Release the **BOOT0** button
4. Connect USB

#### Step 1b: Verify DFU mode
```powershell
dfu-util -l
```
You should see: `Found DFU: [0483:df11] ver=2200, ...`

#### Step 1c: Flash
```powershell
cd "<path-to-xcore407i-remora-project>"
pio run -e xcore407i_eth_dfu -t upload
```

#### Step 1d: Reset and run
Press the **RESET** button. The firmware is now running.

---

### 2. Configure PC Network for Direct Connection

**⚠️ IMPORTANT: This firmware is for direct PC ↔ STM32 connection (no router/switch).**

#### Windows Network Setup

**Option A: PowerShell (Recommended)**
```powershell
# Find the Ethernet adapter name
Get-NetAdapter

# Set static IP (replace "Ethernet" with your adapter name)
New-NetIPAddress -InterfaceAlias "Ethernet" -IPAddress 192.168.10.20 -PrefixLength 24

# If the IP already exists, remove it first:
Remove-NetIPAddress -InterfaceAlias "Ethernet" -IPAddress 192.168.10.20 -Confirm:$false
```

**Option B: GUI (Control Panel)**
1. Open **Control Panel** → **Network and Sharing Center**
2. Click the **Ethernet adapter** → **Properties**
3. Double-click **Internet Protocol Version 4 (TCP/IPv4)**
4. Select **Use the following IP address:**
   ```
   IP address:      192.168.10.20
   Subnet mask:     255.255.255.0
   Default gateway: (empty)
   DNS servers:     (empty)
   ```
5. Click **OK** → **OK**

#### Linux Network Setup

**Temporary (for testing):**
```bash
sudo ip addr add 192.168.10.20/24 dev eth0
```

**Permanent (edit /etc/network/interfaces):**
```bash
auto eth0
iface eth0 inet static
    address 192.168.10.20
    netmask 255.255.255.0
```

---

### 3. Connect Hardware

1. Use an Ethernet cable (straight-through, standard RJ45)
2. Connect: PC Ethernet port ↔ XCore407i RJ45 connector
3. Wait 5 seconds for link negotiation

**💡 Tip:** Most modern Ethernet ports support Auto-MDIX. If it does not work, try a crossover cable.

---

### 4. Test Connectivity

#### Test 1: Ping STM32
```powershell
ping 192.168.10.10
```

**Expected output:**
```
Reply from 192.168.10.10: bytes=32 time<1ms TTL=64
Reply from 192.168.10.10: bytes=32 time<1ms TTL=64
```

✅ **If you get replies, the connection is working.**

❌ **If you get "Request timed out":**
- Check that the PC IP is `192.168.10.20`
- Check the Ethernet cable
- Check PHY LEDs on the board (they should blink)
- Check that STM32 is running the firmware (press RESET)

#### Test 2: ARP Table
```powershell
arp -a | findstr "192.168.10.10"
```

You should see something like:
```
192.168.10.10    00-80-e1-cc-07-01     dynamic
```

---

## 5. LinuxCNC Integration (Next Step)

After confirming connectivity (successful ping), continue with:

### Install and Test `remora-eth` Component

```bash
# Copy driver files to LinuxCNC
cd LinuxCNC/Components/remora-eth
sudo cp remora-eth /usr/bin/
sudo chmod +x /usr/bin/remora-eth

# Test UDP communication
loadusr -W remora-eth

# Check HAL pins
halcmd show pin remora.*
```

### Expected Output
```
remora.joint.0.pos-cmd     OUT float  0.0
remora.joint.0.pos-fb      IN  float  0.0
remora.joint.0.enable      OUT bit    FALSE
remora.alarm.0             IN  bit    FALSE
...
```

---

## Network Configuration Summary

| Device | IP Address | Subnet Mask | Gateway |
|--------|-----------|-------------|---------|
| **STM32 (XCore407i)** | 192.168.10.10 | 255.255.255.0 | - |
| **LinuxCNC PC** | 192.168.10.20 | 255.255.255.0 | (empty) |

**Protocol:** UDP port `27181`  
**Packet Size:** 64 bytes (`rxData`/`txData`)  
**Update Rate:** 1 kHz (servo thread)  
**Connection:** Direct Ethernet cable (no router/switch)

---

## Troubleshooting

### "No DFU device found"
- Check that BOOT0 is HIGH during reset
- Try a different USB port/cable
- Install STM32 DFU drivers (from STM32CubeProgrammer)
- On Windows 10/11, you may need Zadig for WinUSB driver

### "Ping timeout" / "Destination host unreachable"
1. **Check PC IP:**
   ```powershell
   ipconfig | findstr "192.168.10"
   ```
   You should see: `IPv4 Address. . . . . . . . . . . : 192.168.10.20`

2. **Check board PHY LEDs:**
   - **Link LED** (green): solid ON = link OK
   - **Activity LED** (yellow): blinking = data activity

3. **Check Ethernet cable:**
   - Try another cable
   - If using a gigabit PC NIC with a 100 Mbps board, negotiation can fail on some adapters

4. **Restart firmware:**
   - Press RESET on STM32 board
   - Retry ping after 5 seconds

### "Build errors" (if rebuild is needed)
```powershell
# Clean and rebuild
pio run -e xcore407i_eth_dfu -t clean
pio run -e xcore407i_eth_dfu
```

### "UDP packets not received" (after successful ping)
1. **Wireshark capture:**
   ```
   Filter: udp.port == 27181
   ```
   You should see PRU_READ/PRU_WRITE packets while LinuxCNC component is running.

2. **Firewall check:**
   ```powershell
   # Windows: temporarily disable firewall for testing
   netsh advfirewall set allprofiles state off

   # Linux: check iptables
   sudo iptables -L
   ```

3. **Monitor UART (optional debug):**
   - Connect USB-to-Serial: PA2 (TX), PA3 (RX), GND
   - Baud rate: 115200
   - Expected logs include: `UDP bind OK`, `Link up`, etc.

---

## Advanced: Wireshark Packet Analysis

```powershell
# Start capture on the Ethernet interface
# Filter: ip.addr == 192.168.10.10
```

**Expected packets:**
1. **ARP:** PC asks "Who has 192.168.10.10?" → STM32 replies "I'm at 00:80:e1:cc:07:01"
2. **ICMP (ping):** Echo request/reply
3. **UDP (LinuxCNC):** Port 27181 with PRU protocol headers

---

## Useful Commands

```powershell
# Clean build
pio run -e xcore407i_eth_dfu -t clean

# Build with verbose output
pio run -e xcore407i_eth_dfu -v

# Check firmware size
pio run -e xcore407i_eth_dfu -t size

# Monitor serial output (if USB-to-Serial is connected)
pio device monitor -b 115200

# List USB DFU devices
dfu-util -l

# Check PC network interfaces
Get-NetAdapter | Format-Table Name, Status, LinkSpeed
Get-NetIPAddress | Where-Object {$_.IPAddress -like "192.168.10.*"}
```

---

## 🎯 Next Steps

1. ✅ **Flash firmware** (Steps 1-2)
2. ✅ **Setup network** (Step 3)
3. ✅ **Test ping** (Step 4)
4. ⏭️ **LinuxCNC integration** (Step 5)
5. ⏭️ **JSON configuration** and hardware wiring
6. ⏭️ **Closed-loop testing** with encoders and alarms

---

**For support:** If troubleshooting is needed, share output from:
```powershell
ipconfig
ping 192.168.10.10
arp -a | findstr "192.168.10"
```

**Good luck!** 🚀


