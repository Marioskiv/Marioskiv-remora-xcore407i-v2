# Network Setup Guide - Direct PC Connection

## 🔌 Overview

Αυτό το firmware είναι ρυθμισμένο για **απευθείας Ethernet σύνδεση** μεταξύ LinuxCNC PC και STM32 board χωρίς router/switch.

```
┌─────────────┐                    ┌──────────────┐
│ LinuxCNC PC │ <--- Ethernet ---> │ XCore407i    │
│             │      Cable         │ STM32F407    │
│ 192.168.10  │                    │ 192.168.10   │
│       .20   │                    │        .10   │
└─────────────┘                    └──────────────┘
```

**Πλεονεκτήματα:**
- ✅ Χαμηλότερο latency (no router hops)
- ✅ Dedicated bandwidth
- ✅ Απλούστερο setup
- ✅ Ιδανικό για real-time CNC control

---

## 📋 Network Configuration Table

| Parameter | STM32 (XCore407i) | LinuxCNC PC |
|-----------|-------------------|-------------|
| **IP Address** | `192.168.10.10` | `192.168.10.20` |
| **Subnet Mask** | `255.255.255.0` | `255.255.255.0` |
| **Gateway** | (unused) | (empty/none) |
| **DNS** | (unused) | (empty/none) |
| **Protocol** | UDP Server | UDP Client |
| **Port** | 27181 | 27181 |

---

## 🪟 Windows PC Setup

### Option 1: PowerShell (Recommended - Fast)

```powershell
# Step 1: Find your Ethernet adapter name
Get-NetAdapter | Format-Table Name, InterfaceDescription, Status

# Example output:
# Name          InterfaceDescription                   Status
# ----          --------------------                   ------
# Ethernet      Realtek PCIe GbE Family Controller     Up
# Wi-Fi         Intel(R) Wi-Fi 6 AX200 160MHz          Up

# Step 2: Remove existing IP (if any)
Remove-NetIPAddress -InterfaceAlias "Ethernet" -Confirm:$false

# Step 3: Set static IP
New-NetIPAddress -InterfaceAlias "Ethernet" -IPAddress 192.168.10.20 -PrefixLength 24

# Step 4: Verify
Get-NetIPAddress -InterfaceAlias "Ethernet" | Format-Table IPAddress, PrefixLength
```

**Replace "Ethernet"** με το σωστό adapter name από το Step 1.

---

### Option 2: GUI (Control Panel)

1. Πάτα **Win + R**, γράψε `ncpa.cpl`, πάτα Enter
2. Δεξί κλικ στο **Ethernet adapter** → **Properties**
3. Διπλό κλικ στο **Internet Protocol Version 4 (TCP/IPv4)**
4. Επίλεξε **Use the following IP address:**
   ```
   IP address:     192.168.10.20
   Subnet mask:    255.255.255.0
   Default gateway: (leave empty)
   ```
5. Αφήσε **DNS servers** κενά
6. Κλικ **OK** → **OK** → **Close**

---

### Option 3: netsh (Command Prompt)

```cmd
REM Find interface name
netsh interface show interface

REM Set static IP (replace "Ethernet" with your interface name)
netsh interface ip set address name="Ethernet" static 192.168.10.20 255.255.255.0

REM Verify
netsh interface ip show address "Ethernet"
```

---

## 🐧 Linux PC Setup

### Temporary Setup (lost after reboot - για testing)

```bash
# Add IP to interface (replace eth0 with your interface name)
sudo ip addr add 192.168.10.20/24 dev eth0

# Verify
ip addr show eth0 | grep "192.168.10"

# Remove IP (αν χρειαστεί)
sudo ip addr del 192.168.10.20/24 dev eth0
```

---

### Permanent Setup (survives reboot)

#### Method 1: /etc/network/interfaces (Debian/Ubuntu old style)

```bash
sudo nano /etc/network/interfaces
```

Add:
```
auto eth0
iface eth0 inet static
    address 192.168.10.20
    netmask 255.255.255.0
```

Restart networking:
```bash
sudo systemctl restart networking
```

---

#### Method 2: Netplan (Ubuntu 18.04+)

```bash
sudo nano /etc/netplan/01-netcfg.yaml
```

Add/modify:
```yaml
network:
  version: 2
  renderer: networkd
  ethernets:
    eth0:
      dhcp4: no
      addresses:
        - 192.168.10.20/24
```

Apply:
```bash
sudo netplan apply
```

---

#### Method 3: NetworkManager (LinuxCNC Debian default)

```bash
# GUI: Right-click network icon → Edit Connections → Select Ethernet → Edit
# Set IPv4 Method: Manual
# Address: 192.168.10.20
# Netmask: 255.255.255.0
# Gateway: (empty)

# CLI alternative:
nmcli con mod "Wired connection 1" ipv4.addresses 192.168.10.20/24
nmcli con mod "Wired connection 1" ipv4.method manual
nmcli con down "Wired connection 1" && nmcli con up "Wired connection 1"
```

---

## 🔌 Hardware Connection

### Required Hardware
- **1x Ethernet Cable** (Cat5e or better, straight-through or crossover)
- **XCore407i board** with DP83848I PHY
- **PC with Ethernet port**

### Connection Steps
1. Power ON το STM32 board
2. Plug Ethernet cable: **PC Ethernet port** ↔ **XCore407i RJ45 connector**
3. Περίμενε **5-10 δευτερόλεπτα** για:
   - Link negotiation (10/100 Mbps auto-detect)
   - ARP table population

### Cable Type Notes
- **Modern systems (Auto-MDIX)**: Οποιοδήποτε cable λειτουργεί (straight-through ή crossover)
- **Older systems (no Auto-MDIX)**: Χρειάζεσαι **crossover cable** για PC-to-device direct connection
- **Check Link LEDs**:
  - **Green LED (solid)**: Link established
  - **Yellow LED (blinking)**: Data activity

---

## ✅ Testing & Verification

### Test 1: Ping STM32 from PC

**Windows:**
```powershell
ping 192.168.10.10 -t
```

**Linux:**
```bash
ping 192.168.10.10
```

**Expected Output:**
```
Reply from 192.168.10.10: bytes=32 time<1ms TTL=64
Reply from 192.168.10.10: bytes=32 time<1ms TTL=64
Reply from 192.168.10.10: bytes=32 time<1ms TTL=64
```

✅ **Success:** Latency < 1ms, no packet loss  
❌ **Failure:** "Request timed out" or "Destination host unreachable"

---

### Test 2: Check ARP Table

**Windows:**
```powershell
arp -a | findstr "192.168.10.10"
```

**Linux:**
```bash
arp -a | grep "192.168.10.10"
```

**Expected Output:**
```
192.168.10.10    00-80-e1-cc-07-01    dynamic
```

This confirms MAC address resolution is working.

---

### Test 3: Check PC IP Configuration

**Windows:**
```powershell
ipconfig | findstr "192.168.10"
```

**Linux:**
```bash
ip addr show | grep "192.168.10"
```

**Expected Output:**
```
IPv4 Address. . . . . . . . . . . : 192.168.10.20
Subnet Mask . . . . . . . . . . . : 255.255.255.0
```

---

### Test 4: Traceroute (Optional)

**Windows:**
```powershell
tracert -d 192.168.10.10
```

**Linux:**
```bash
traceroute -n 192.168.10.10
```

**Expected:** Only **1 hop** (direct connection)

---

### Test 5: UDP Port Check (Advanced)

**Linux (netcat):**
```bash
# Listen on PC (testing if PC can receive UDP)
nc -u -l 27181

# Send test packet from another terminal
echo "test" | nc -u 192.168.10.10 27181
```

**Windows (Test-NetConnection - PowerShell 4.0+):**
```powershell
# Only works for TCP, but checks basic connectivity
Test-NetConnection -ComputerName 192.168.10.10 -InformationLevel Detailed
```

---

## 🔧 Troubleshooting

### Problem: "Request timed out" / No ping reply

**Checklist:**

1. **PC IP correct?**
   ```powershell
   ipconfig
   ```
   Verify: `192.168.10.20` on correct interface

2. **Ethernet cable plugged in?**
   - Check both ends firmly seated
   - Try different cable
   - Check for physical damage

3. **PHY LEDs on XCore407i?**
   - **Link LED**: Should be ON (solid green)
   - **Activity LED**: Should blink (yellow/orange)
   - If LEDs OFF: Check board power, PHY reset

4. **Firewall blocking ICMP?**
   
   **Windows:**
   ```powershell
   # Temporarily disable (TESTING ONLY)
   netsh advfirewall set allprofiles state off
   
   # Re-enable after testing
   netsh advfirewall set allprofiles state on
   ```
   
   **Linux:**
   ```bash
   # Check firewall rules
   sudo iptables -L
   
   # Temporarily allow ICMP
   sudo iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
   ```

5. **STM32 firmware running?**
   - Press **RESET** button on board
   - Wait 5 seconds
   - Retry ping

6. **Wrong interface selected?**
   ```powershell
   # List all interfaces with IPs
   Get-NetAdapter | Format-Table Name, Status, LinkSpeed
   ```
   Ethernet adapter should show **Status: Up** and **LinkSpeed: 100 Mbps** or **1 Gbps**

---

### Problem: "Destination host unreachable"

**Meaning:** PC can't find a route to 192.168.10.10

**Solutions:**

1. **Check subnet mask:**
   ```powershell
   netsh interface ip show address
   ```
   Must be `255.255.255.0` (both PC and STM32)

2. **Check routing table:**
   
   **Windows:**
   ```powershell
   route print | findstr "192.168.10"
   ```
   
   **Linux:**
   ```bash
   ip route show | grep "192.168.10"
   ```
   
   Should see: `192.168.10.0/24 dev eth0 scope link`

3. **Delete conflicting routes:**
   
   **Windows:**
   ```powershell
   route delete 192.168.10.0
   # Then re-add IP
   New-NetIPAddress -InterfaceAlias "Ethernet" -IPAddress 192.168.10.20 -PrefixLength 24
   ```

---

### Problem: ARP entry shows incomplete or wrong MAC

**Symptoms:**
```
192.168.10.10    <incomplete>
```

**Solutions:**

1. **Clear ARP cache:**
   
   **Windows:**
   ```powershell
   arp -d 192.168.10.10
   netsh interface ip delete arpcache
   ```
   
   **Linux:**
   ```bash
   sudo ip neigh flush all
   ```

2. **Check for MAC conflicts:**
   ```powershell
   arp -a
   ```
   Verify no duplicate MAC addresses

3. **STM32 MAC address:** Default should be `00:80:E1:CC:07:01` (check `netconf.c`)

---

### Problem: Link LEDs not lighting

**PHY not initializing - Hardware issue:**

1. **Check power rails:**
   - 3.3V present at PHY chip?
   - Multimeter test at DP83848I pins

2. **Check PHY reset:**
   - PG13 (PHY_RST) should be HIGH during normal operation
   - Scope/multimeter check

3. **Check RMII clock:**
   - PA1 (REF_CLK) should have 50 MHz signal
   - Scope check (expect clean 50 MHz square wave)

4. **Check crystal/oscillator:**
   - 25 MHz clock to DP83848I XI pin
   - Or 50 MHz to DP83848I X1 (depending on board design)

---

### Problem: Windows "Unidentified network"

**Symptoms:** Windows marks Ethernet as "Unidentified network" and restricts access

**Solution:**

1. **Set network location:**
   ```powershell
   # PowerShell as Administrator
   Set-NetConnectionProfile -InterfaceAlias "Ethernet" -NetworkCategory Private
   ```

2. **Or via GUI:**
   - Settings → Network & Internet → Ethernet
   - Click on connection name
   - Change to "Private"

3. **If still blocked, allow in firewall:**
   ```powershell
   New-NetFirewallRule -DisplayName "Remora CNC" -Direction Inbound -Protocol UDP -LocalPort 27181 -Action Allow
   New-NetFirewallRule -DisplayName "Remora CNC" -Direction Outbound -Protocol UDP -RemotePort 27181 -Action Allow
   ```

---

## 🌐 Alternative: Router/Switch Connection

Αν **δεν** θέλεις direct connection και προτιμάς router/switch:

### STM32 Firmware Changes Required

Edit `netconf.c` (or create `netconf.h` override):

```c
// Change IP to match your network subnet
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1    // Changed from 10
#define IP_ADDR3   100  // Changed from 10

// Change gateway to your router IP
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1    // Router IP
```

Rebuild & reflash firmware.

### PC Configuration

Set PC to **DHCP** or static IP in **192.168.1.x** range (matching your router).

### LinuxCNC Driver Update

Edit `remora-eth-3.0.c`:
```c
const char* dstAddress = "192.168.1.100";  // New STM32 IP
```

---

## 📊 Network Performance Monitoring

### Wireshark Packet Capture

1. **Start capture** on Ethernet interface
2. **Filter:** `ip.addr == 192.168.10.10`
3. **Expected packets:**
   - **ARP:** Who has 192.168.10.10? → MAC response
   - **ICMP:** Echo request/reply (from ping)
   - **UDP:** Port 27181 with PRU protocol (from LinuxCNC)

### Check Packet Loss

**Continuous ping test:**
```powershell
# Windows (Ctrl+C to stop)
ping 192.168.10.10 -t

# Linux (Ctrl+C to stop)
ping 192.168.10.10
```

**Expected:** 0% packet loss, latency < 1ms consistently

**If packet loss:**
- Check cable quality
- Check PHY duplex/speed negotiation
- Check for electrical interference near cable

---

## 📝 Quick Reference Card

### IP Configuration
```
STM32:     192.168.10.10/24
PC:        192.168.10.20/24
Protocol:  UDP port 27181
Cable:     Direct Ethernet (straight-through or crossover)
```

### Test Commands
```powershell
# Verify PC IP
ipconfig | findstr "192.168.10"

# Test connectivity
ping 192.168.10.10

# Check ARP
arp -a | findstr "192.168.10.10"

# Check interface status
Get-NetAdapter | Where-Object {$_.Status -eq "Up"}
```

### Firewall (if needed)
```powershell
# Windows: Allow UDP 27181
New-NetFirewallRule -DisplayName "Remora" -Direction Inbound -Protocol UDP -LocalPort 27181 -Action Allow

# Linux: Allow UDP 27181
sudo ufw allow 27181/udp
```

---

## 🆘 Getting Help

If still having issues, gather this info:

```powershell
# Windows diagnostic bundle
ipconfig /all > network_info.txt
arp -a >> network_info.txt
ping 192.168.10.10 >> network_info.txt
route print >> network_info.txt
Get-NetAdapter | Format-Table * >> network_info.txt
```

```bash
# Linux diagnostic bundle
ip addr > network_info.txt
ip route >> network_info.txt
arp -a >> network_info.txt
ping -c 5 192.168.10.10 >> network_info.txt
```

Post `network_info.txt` when asking for help!

---

**Last Updated:** October 26, 2025  
**Firmware Version:** UDP Comms Implementation  
**Compatible with:** LinuxCNC remora-eth driver (6-axis version)
