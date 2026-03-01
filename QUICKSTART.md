# Quick Start Guide - XCore407I Remora Firmware

Get your XCore407I board running with LinuxCNC in under 10 minutes!

## ✅ What's Ready

✅ **Firmware compiled and tested**  
✅ **Ethernet communication implemented**  
✅ **LinuxCNC configurations provided**  
✅ **Documentation complete**  

---

## 🚀 5-Minute Setup

### 1. Flash Firmware

#### Step 1a: Put board in DFU mode
1. Hold **BOOT0** button
2. Press and release **RESET** button  
3. Release **BOOT0** button
4. Connect USB cable

#### Step 1b: Verify DFU mode
```bash
dfu-util -l
```
Should show: `Found DFU: [0483:df11] ver=2200, ...`

#### Step 1c: Flash
```bash
cd "xcore407i-remora"
platformio run -e xcore407i_eth_dfu -t upload
```

#### Step 1d: Reset & Run
Press **RESET** button. Firmware is now running!
```powershell
cd "C:\Users\mario\OneDrive\Έγγραφα\PlatformIO\Projects\xcore407i stm32f407igt6 REMORA"
pio run -e xcore407i_eth_dfu -t upload
```

#### Βήμα 1d: Reset & Run
Πάτα **RESET** button. Το firmware τρέχει τώρα!

---

### 2. Ρύθμισε PC Network για Direct Connection

**⚠️ ΣΗΜΑΝΤΙΚΟ: Αυτό το firmware είναι για ΑΠΕΥΘΕΙΑΣ σύνδεση PC ↔ STM32 (χωρίς router/switch)**

#### Windows Network Setup

**Option A: PowerShell (Recommended)**
```powershell
# Βρες το Ethernet adapter name
Get-NetAdapter

# Set static IP (αντικατάστησε "Ethernet" με το σωστό όνομα)
New-NetIPAddress -InterfaceAlias "Ethernet" -IPAddress 192.168.10.20 -PrefixLength 24

# Αν υπάρχει ήδη IP, πρώτα remove:
Remove-NetIPAddress -InterfaceAlias "Ethernet" -IPAddress 192.168.10.20 -Confirm:$false
```

**Option B: GUI (Control Panel)**
1. Άνοιξε **Control Panel** → **Network and Sharing Center**
2. Κλικ στο **Ethernet adapter** → **Properties**
3. Διπλό κλικ στο **Internet Protocol Version 4 (TCP/IPv4)**
4. Επίλεξε **Use the following IP address:**
   ```
   IP address:     192.168.10.20
   Subnet mask:    255.255.255.0
   Default gateway: (άδειο)
   DNS servers:     (άδειο)
   ```
5. Κλικ **OK** → **OK**

#### Linux Network Setup

**Temporary (για testing):**
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

### 3. Σύνδεση Hardware

1. **Πάρε ένα Ethernet cable** (straight-through, standard RJ45)
2. **Σύνδεσε**: PC Ethernet port ↔ XCore407i RJ45 connector
3. **Περίμενε 5 δευτερόλεπτα** για link negotiation

**💡 Tip:** Τα περισσότερα σύγχρονα Ethernet ports έχουν Auto-MDIX. Αν δεν λειτουργεί, δοκίμασε **crossover cable**.

---

### 4. Test Connectivity

#### Test 1: Ping το STM32
```powershell
ping 192.168.10.10
```

**Αναμενόμενο:**
```
Reply from 192.168.10.10: bytes=32 time<1ms TTL=64
Reply from 192.168.10.10: bytes=32 time<1ms TTL=64
```

✅ **Αν βλέπεις replies → Η σύνδεση λειτουργεί!**

❌ **Αν "Request timed out":**
- Έλεγξε ότι το PC έχει IP 192.168.10.20
- Έλεγξε το Ethernet cable
- Κοίταξε PHY LEDs στο board (πρέπει να αναβοσβήνουν)
- Έλεγξε ότι το STM32 έτρεχε το firmware (πάτα RESET)

#### Test 2: ARP Table
```powershell
arp -a | findstr "192.168.10.10"
```

Πρέπει να δεις κάτι σαν:
```
192.168.10.10    00-80-e1-cc-07-01     dynamic
```

---

---

## 5. LinuxCNC Integration (Επόμενο βήμα)

Αφού επιβεβαιώσεις ότι η σύνδεση λειτουργεί (ping success), το επόμενο βήμα είναι:

### Install & Test remora-eth Component

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

### Expected Output:
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

**Protocol:** UDP port 27181  
**Packet Size:** 64 bytes (rxData/txData)  
**Update Rate:** 1 kHz (servo thread)  
**Connection:** Direct Ethernet cable (no router/switch)

---

## Troubleshooting

### "No DFU device found"
- Έλεγξε ότι BOOT0 είναι HIGH κατά το reset
- Δοκίμασε άλλο USB port/cable
- Install STM32 DFU drivers (από STM32CubeProgrammer)
- Στα Windows 10/11 μπορεί να χρειαστεί Zadig για WinUSB driver

### "Ping timeout" / "Destination host unreachable"
1. **Έλεγξε PC IP:**
   ```powershell
   ipconfig | findstr "192.168.10"
   ```
   Πρέπει να δεις: `IPv4 Address. . . . . . . . . . . : 192.168.10.20`

2. **Έλεγξε PHY LEDs στο board:**
   - **Link LED** (πράσινο): Σταθερό αναμμένο = καλή σύνδεση
   - **Activity LED** (κίτρινο): Αναβοσβήνει = data transmission

3. **Έλεγξε Ethernet cable:**
   - Δοκίμασε διαφορετικό cable
   - Αν έχεις gigabit PC + 100Mbps board, μπορεί να υπάρχει negotiation issue

4. **Restart firmware:**
   - Πάτα RESET button στο STM32 board
   - Ξανά-δοκίμασε ping μετά από 5 δευτερόλεπτα

### "Build errors" (αν χρειαστείς rebuild)
```powershell
# Clean & rebuild
pio run -e xcore407i_eth_dfu -t clean
pio run -e xcore407i_eth_dfu
```

### "UDP packets not received" (μετά από επιτυχή ping)
1. **Wireshark capture:**
   ```
   Filter: udp.port == 27181
   ```
   Θα δεις PRU_READ/PRU_WRITE packets αν το LinuxCNC component τρέχει

2. **Firewall check:**
   ```powershell
   # Windows: Temporarily disable firewall για testing
   netsh advfirewall set allprofiles state off
   
   # Linux: Check iptables
   sudo iptables -L
   ```

3. **Monitor UART (optional debug):**
   - Σύνδεσε USB-to-Serial: PA2 (TX), PA3 (RX), GND
   - Baud: 115200
   - Θα δεις: "UDP bind OK", "Link up", etc.

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

## Χρήσιμες Εντολές

```powershell
# Clean build
pio run -e xcore407i_eth_dfu -t clean

# Build με verbose output
pio run -e xcore407i_eth_dfu -v

# Check μέγεθος firmware
pio run -e xcore407i_eth_dfu -t size

# Monitor serial output (αν έχεις USB-to-Serial)
pio device monitor -b 115200

# List USB DFU devices
dfu-util -l

# Check PC network interfaces
Get-NetAdapter | Format-Table Name, Status, LinkSpeed
Get-NetIPAddress | Where-Object {$_.IPAddress -like "192.168.10.*"}
```

---

## 🎯 Επόμενα Βήματα

1. ✅ **Flash firmware** (Βήματα 1-2)
2. ✅ **Setup network** (Βήμα 3)
3. ✅ **Test ping** (Βήμα 4)
4. ⏭️ **LinuxCNC integration** (Βήμα 5)
5. ⏭️ **JSON configuration** & hardware wiring
6. ⏭️ **Closed-loop testing** με encoders & alarms

---

**Για βοήθεια:** Αν χρειαστείς troubleshooting, στείλε το output από:
```powershell
ipconfig
ping 192.168.10.10
arp -a | findstr "192.168.10"
```

**Καλή επιτυχία!** 🚀


