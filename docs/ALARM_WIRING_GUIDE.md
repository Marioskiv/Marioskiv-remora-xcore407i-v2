# Οδηγός Συνδεσμολογίας Motor Alarm Signals (Closed-Loop Servo)

## ⚡ Quick Reference - Differential Alarm (A+/A-)

**ΤΟ SERVO ΣΟΥ ΕΧΕΙ:** 2 καλώδια alarm (A+, A-) με photoelectric isolation, open collector output

### ✅ Σωστή Σύνδεση:
```
Servo Motor Alarm:
    A+ (Alarm+) ──────> +3.3V (STM32 power pin, π.χ. P2-37)
    A- (Alarm-) ──────> GPIO Pin (π.χ. PC_0 με pullup enabled)

JSON Config:
    "Pullup": true
    "Invert": true    // Active-Low: alarm = SHORT (A+ to A-) = LOW
```

### ❌ Λάθος Σύνδεση (ΜΗΝ το κάνεις):
```
Servo Motor Alarm:
    A+ ──X──> GPIO Pin (PC_0)  ← ΛΑΘΟΣ! Θα κάψεις το pin!
    A- ──X──> GND              ← ΛΑΘΟΣ! Δεν θα δουλέψει!
```

### 🔌 Pin Assignment (8 άξονες):
| Axis | A+ → | A- → | Bit |
|------|------|------|-----|
| X (J0) | +3.3V | PC_0 | 16 |
| Y (J1) | +3.3V | PC_1 | 17 |
| Z (J2) | +3.3V | PC_2 | 18 |
| A (J3) | +3.3V | PC_3 | 19 |
| B (J4) | +3.3V | PF_6 | 20 |
| C (J5) | +3.3V | PF_7 | 21 |
| D (J6) | +3.3V | PF_8 | 22 |
| E (J7) | +3.3V | PF_9 | 23 |

**Όλα τα A+ μοιράζονται το ίδιο +3.3V pin!**

---

## 1. Τι είναι το Motor Alarm Signal;

Τα **closed-loop servo motors** (AC/DC servo με encoder) έχουν συνήθως έξοδο **ALARM** (ή **ALM**, **ERR**, **FAULT**) που ενεργοποιείται όταν ο driver ανιχνεύσει πρόβλημα:

### Συνήθη Προβλήματα που Ανιχνεύονται:
- ✅ **Encoder Error**: Πρόβλημα στον encoder (θόρυβος, κομμένο καλώδιο, βραχυκύκλωμα)
- ✅ **Overcurrent**: Υπερφόρτωση κινητήρα (κόλλημα, υπερβολική φόρτωση)
- ✅ **Position Error** (internal): Το servo δεν μπορεί να ακολουθήσει την εντολή
- ✅ **Overvoltage/Undervoltage**: Πρόβλημα στην τροφοδοσία
- ✅ **Overtemperature**: Υπερθέρμανση driver ή motor
- ✅ **Communication Error**: Απώλεια επικοινωνίας με motor (RS485, CANopen, etc.)

---

## 2. Τύποι Alarm Signals

### A. Differential Photoelectric Isolated (A+, A-) - **ΣΥΝΗΘΕΣΤΕΡΟ ΣΕ INDUSTRIAL SERVO**
```
Servo Motor Alarm Output (Photoelectric Isolated, OC)
         
    A+ ────────┐
               │  [Optocoupler inside servo driver]
    A- ────────┘  (Open Collector Output, isolated)
```

**Χαρακτηριστικά:**
- **2 καλώδια**: A+ (Alarm+), A- (Alarm-)
- **Photoelectric Isolation**: Galvanic isolation μεταξύ servo και control
- **Open Collector (OC)**: Χρειάζεται external pullup
- **Active-Low**: Κανονικά OPEN, σε alarm SHORT (A+ → A-)

#### Σύνδεση με STM32 (Recommended):

**Μέθοδος 1: Single-Ended με Pullup (Απλούστερο)**
```
Servo Alarm Output:
    A+ ──────────────> +3.3V (STM32)
    A- ──────────────> PC_0 (STM32 GPIO)
                       |
                   [10kΩ Pullup] (internal)
                       |
                     +3.3V
```

**Λειτουργία:**
- **Κανονική**: A+/A- OPEN → PC_0 = HIGH (3.3V μέσω pullup)
- **Alarm**: A+/A- SHORT → PC_0 = LOW (0V, pulled to A+ = 3.3V)

**JSON Config:**
```json
{
  "Type": "Alarm",
  "Pin": "PC_0",
  "Invert": true,      // LOW = Alarm
  "Pullup": true       // Ενεργοποιεί internal pullup
}
```

---

**Μέθοδος 2: External Pullup (Για μακριά καλώδια ή θόρυβο)**
```
Servo Alarm Output:
    A+ ──────────────> +3.3V (STM32)
    A- ──────[ 4.7kΩ ]───> PC_0 (STM32 GPIO)
                            |
                        [4.7kΩ External Pullup]
                            |
                          +3.3V

Optional: RC filter για noise immunity
    PC_0 ──[ 100Ω ]──┬── STM32 actual pin
                     │
                   [100nF]
                     │
                    GND
```

**Πλεονεκτήματα:**
- Stronger pullup (4.7kΩ vs 40kΩ internal)
- Καλύτερη noise immunity με RC filter
- Recommended για καλώδια > 2 μέτρα

---

**Μέθοδος 3: Differential Input με Comparator (Professional, δεν χρειάζεται για STM32)**
```
    A+ ──[ 10kΩ ]──┬──> Comparator (+)
                   │
    A- ──[ 10kΩ ]──┼──> Comparator (-)
                   │
                  GND

    Comparator Output ──> PC_0 (STM32)
```
Αυτό είναι overkill για τα περισσότερα συστήματα.

---

### B. Single-Ended Active-Low (NPN Open Collector) - **ΛΙΓΟΤΕΡΟ ΣΥΝΗΘΗΣ**
```
Servo Driver Alarm Output (Single NPN Open Collector)
         |
         |---- [Optional 1kΩ Resistor]
         |
    STM32 Pin (π.χ. PC_0)
         |
    (Internal Pullup enabled)
```

**Λειτουργία:**
- **Κανονική λειτουργία**: Transistor OFF → Pin = HIGH (3.3V μέσω pullup)
- **Alarm Active**: Transistor ON → Pin = LOW (0V, βραχυκύκλωμα στη γη)

**JSON Config:**
```json
{
  "Type": "Alarm",
  "Pin": "PC_0",
  "Invert": true,      // LOW = Alarm
  "Pullup": true       // Ενεργοποιεί pullup
}
```

### B. Active-High (PNP or Push-Pull) - **ΣΠΑΝΙΟ ΣΕ SERVO**
```
Servo Driver Alarm Output (PNP or Push-Pull)
         |
         |---- [Optional 1kΩ Resistor]
         |
    STM32 Pin (π.χ. PC_0)
         |
    (No pullup needed)
```

**Λειτουργία:**
- **Κανονική λειτουργία**: Output = LOW (0V)
- **Alarm Active**: Output = HIGH (5V ή 24V → **χρειάζεται level shifter!**)

**ΠΡΟΣΟΧΗ:** Αν το alarm είναι 5V ή 24V, **ΜΗΝ** το συνδέσεις απευθείας στο STM32 (3.3V max)!

**Λύση:** Χρησιμοποίησε:
- Voltage divider (π.χ. 2kΩ + 1kΩ για 5V → 3.3V)
- Optocoupler (π.χ. TLP281, PC817)
- Logic level converter module

**JSON Config:**
```json
{
  "Type": "Alarm",
  "Pin": "PC_0",
  "Invert": false,     // HIGH = Alarm
  "Pullup": false      // Δεν χρειάζεται pullup
}
```

---

## 3. Σύγκριση Τύπων Alarm Signals

| Τύπος | Καλώδια | Isolation | Pullup | Invert | Χρήση |
|-------|---------|-----------|--------|--------|-------|
| **Differential A+/A-** | 2 | ✅ Photoelectric | Required | true | Industrial servo (Mitsubishi, Yaskawa, etc.) |
| **Single-Ended NPN** | 1 + GND | ❌ | Required | true | Budget servo, stepper με alarm |
| **Active-High** | 1 + GND | ❌ | Not needed | false | Σπάνιο |

**Το servo motor σου έχει A+/A- → Χρησιμοποίησε Μέθοδο 1 (απλούστερο) ή Μέθοδο 2 (για θόρυβο)**

Από το **PIN_ASSIGNMENT.md**:

| Axis | Alarm Pin | Data Bit | STM32 Header |
|------|-----------|----------|--------------|
| X (Joint 0) | **PC_0** | 16 | P1-?? |
| Y (Joint 1) | **PC_1** | 17 | P1-?? |
| Z (Joint 2) | **PC_2** | 18 | P1-?? |
| A (Joint 3) | **PC_3** | 19 | P1-?? |
| B (Joint 4) | **PF_6** | 20 | P2-?? |
| C (Joint 5) | **PF_7** | 21 | P2-?? |
| D (Joint 6) | **PF_8** | 22 | P2-?? |
| E (Joint 7) | **PF_9** | 23 | P2-?? |

**Σημείωση:** Συμβουλέψου το schematic (`resources/schematic xcore407i/xcore407i_pin_headers.json`) για να βρεις τον ακριβή αριθμό pin στα headers P1/P2.

---

## 4. Pins για Motor Alarms (8 άξονες)

Από το **PIN_ASSIGNMENT.md**:

| Axis | Alarm Pin | A+ Connection | A- Connection | Data Bit |
|------|-----------|---------------|---------------|----------|
| X (Joint 0) | **PC_0** | +3.3V | PC_0 | 16 |
| Y (Joint 1) | **PC_1** | +3.3V | PC_1 | 17 |
| Z (Joint 2) | **PC_2** | +3.3V | PC_2 | 18 |
| A (Joint 3) | **PC_3** | +3.3V | PC_3 | 19 |
| B (Joint 4) | **PF_6** | +3.3V | PF_6 | 20 |
| C (Joint 5) | **PF_7** | +3.3V | PF_7 | 21 |
| D (Joint 6) | **PF_8** | +3.3V | PF_8 | 22 |
| E (Joint 7) | **PF_9** | +3.3V | PF_9 | 23 |

**Σημειώσεις:**
- **A+ (Alarm+)**: Συνδέεται στο **+3.3V** του STM32 (όχι στο GPIO pin!)
- **A- (Alarm-)**: Συνδέεται στο **GPIO pin** (π.χ. PC_0) με pullup enabled
- **Pullup**: Ενεργοποιημένο στο JSON config (`Pullup: true`)
- **Invert**: `true` γιατί είναι active-low (alarm = SHORT = LOW)

**Σημαντικό:** Μην συνδέσεις το A+ στο GPIO pin - θα κάψεις το MCU! Το A+ πάει στο +3.3V.

---

## 5. Παράδειγμα Συνδεσμολογίας (Differential A+/A-)

### Hardware Connection (Μέθοδος 1 - Απλή):
```
┌─────────────────────────────┐
│  Servo Motor Driver         │
│                             │
│  Alarm A+ (isolated OC) ────┼───────> +3.3V (STM32 power pin)
│                             │
│  Alarm A- (isolated OC) ────┼───────> PC_0 (STM32 GPIO)
│                             │         |
│                             │     [10kΩ Pullup] (internal)
│                             │         |
└─────────────────────────────┘       +3.3V

Κανονική λειτουργία: A+/A- OPEN  → PC_0 = HIGH (3.3V)
Alarm:               A+/A- SHORT → PC_0 = LOW  (0V)
```
```
┌─────────────────────┐
│  Servo Driver       │
│                     │
│  ALM (NPN)  ────────┼────> PC_0 (STM32)
│                     │      (Pullup enabled)
│  GND ───────────────┼────> GND
│                     │
└─────────────────────┘
```

### Χωρίς Optocoupler (Recommended αν driver έχει isolated output):
### Hardware Connection (Μέθοδος 1 - Απλή):
```
┌─────────────────────────────┐
│  Servo Motor Driver         │
│                             │
│  Alarm A+ (isolated OC) ────┼───────> +3.3V (STM32 power pin)
│                             │
│  Alarm A- (isolated OC) ────┼───────> PC_0 (STM32 GPIO)
│                             │         |
│                             │     [10kΩ Pullup] (internal)
│                             │         |
└─────────────────────────────┘       +3.3V

Κανονική λειτουργία: A+/A- OPEN  → PC_0 = HIGH (3.3V)
Alarm:               A+/A- SHORT → PC_0 = LOW  (0V)
```

### Hardware Connection (Μέθοδος 2 - Με External Pullup και RC Filter):
```
┌─────────────────────────────┐
│  Servo Motor Driver         │
│                             │
│  Alarm A+ (isolated OC) ────┼───────> +3.3V (STM32)
│                             │
│  Alarm A- (isolated OC) ────┼────┬──[ 100Ω ]──┬──> PC_0 (STM32 GPIO)
│                             │    │            │
└─────────────────────────────┘    │          [100nF]
                                   │            │
                             [4.7kΩ Pullup]    GND
                                   │
                                 +3.3V

RC Filter: 100Ω + 100nF = cutoff ~16 kHz (φιλτράρει θόρυβο, διατηρεί σήμα)
External Pullup: 4.7kΩ (πιο δυνατό από το internal 40kΩ)
```

**Πότε να χρησιμοποιήσεις Μέθοδο 2:**
- ✅ Καλώδιο alarm > 2 μέτρα
- ✅ Υπάρχει θόρυβος από step/dir signals (EMI)
- ✅ False alarms (random triggers)
- ✅ Εργοστασιακό περιβάλλον με πολλά μηχανήματα

**Για τα περισσότερα συστήματα, η Μέθοδος 1 είναι αρκετή.**

---

### Wiring Checklist:

**ΓΙΑ ΚΑΘΕ SERVO (Joints 0-7):**

1. **Βρες τα καλώδια A+/A-** στον connector του servo motor
   - Συνήθως: 
     - Κόκκινο/Πορτοκαλί = A+ (Alarm+)
     - Μαύρο/Μπλε = A- (Alarm-)
   - Δες το manual του servo για το pinout!

2. **Σύνδεσε το A+ στο +3.3V του STM32**
   - Χρησιμοποίησε το P2-37 ή P2-69 pin (3.3V στο header)
   - **ΜΗΝ** το συνδέσεις στο GPIO pin!

3. **Σύνδεσε το A- στο GPIO pin**
   - Joint 0 (X): A- → PC_0
   - Joint 1 (Y): A- → PC_1
   - Joint 2 (Z): A- → PC_2
   - κ.λπ. (δες πίνακα παραπάνω)

4. **Ενεργοποίησε το Pullup στο JSON config**
   ```json
   "Pullup": true
   ```

5. **Set Invert = true**
   ```json
   "Invert": true
   ```

---

### Παράδειγμα για 8 Servos:
```
STM32 Board:
    +3.3V (P2-37) ──┬─── Servo 0 Alarm A+
                    ├─── Servo 1 Alarm A+
                    ├─── Servo 2 Alarm A+
                    ├─── Servo 3 Alarm A+
                    ├─── Servo 4 Alarm A+
                    ├─── Servo 5 Alarm A+
                    ├─── Servo 6 Alarm A+
                    └─── Servo 7 Alarm A+

    PC_0 ───────────── Servo 0 Alarm A-
    PC_1 ───────────── Servo 1 Alarm A-
    PC_2 ───────────── Servo 2 Alarm A-
    PC_3 ───────────── Servo 3 Alarm A-
    PF_6 ───────────── Servo 4 Alarm A-
    PF_7 ───────────── Servo 5 Alarm A-
    PF_8 ───────────── Servo 6 Alarm A-
    PF_9 ───────────── Servo 7 Alarm A-

Σημείωση: Όλα τα A+ μοιράζονται το ίδιο +3.3V (common)
```

---

### ❌ ΛΑΘΟΣ Σύνδεση (ΜΗΝ το κάνεις):
```
Servo Alarm A+ ──X──> PC_0 (GPIO)  ← ΛΑΘΟΣ! Θα κάψεις το pin!
Servo Alarm A- ──X──> GND          ← ΛΑΘΟΣ! Δεν θα δουλέψει!
```

**Σωστό:**
```
Servo Alarm A+ ──✓──> +3.3V (Power)
Servo Alarm A- ──✓──> PC_0 (GPIO με pullup)
```

---

## 6. Έλεγχος Συνδεσμολογίας

### Α. Με Multimeter:
1. **Κανονική λειτουργία** (δεν υπάρχει alarm):
   - Active-Low: Μέτρησε **~3.3V** στο pin
   - Active-High: Μέτρησε **0V** στο pin

2. **Trigger alarm** (π.χ. disable encoder, overcurrent):
   - Active-Low: Μέτρησε **0V** στο pin
   - Active-High: Μέτρησε **~3.3V** στο pin (ή 5V αν χρειάζεται level shifter)

### Β. Με Firmware Debug:
```cpp
// Στο alarm.cpp, υπάρχει ήδη:
printf("ALARM: Joint %d alarm triggered! (Count: %lu)\n", jointNumber, alarmCount);
```

Άνοιξε serial monitor (115200 baud) και trigger το alarm:
```
ALARM: Joint 0 alarm triggered! (Count: 1)
Joint 0 in ALARM state
```

### Γ. Με LinuxCNC HAL:
```bash
# Άνοιξε HAL meter
halmeter

# Watch:
remora.input.16    # Για Joint 0 alarm (bit 16)

# Τιμή:
# FALSE (0) = Κανονική λειτουργία
# TRUE (1) = Alarm active
```

---

## 6. LinuxCNC HAL Configuration

### Α. Διάβασε τα Alarm Bits:
```hal
# Joint 0-7 Motor Alarms (bits 16-23)
net j0-motor-alarm    remora.input.16
net j1-motor-alarm    remora.input.17
net j2-motor-alarm    remora.input.18
net j3-motor-alarm    remora.input.19
net j4-motor-alarm    remora.input.20
net j5-motor-alarm    remora.input.21
net j6-motor-alarm    remora.input.22
net j7-motor-alarm    remora.input.23
```

### Β. Συνδυασμός με Position Monitor Faults:
```hal
# Position Monitor Faults (bits 8-15)
net j0-pos-fault      remora.input.8
net j1-pos-fault      remora.input.9
net j2-pos-fault      remora.input.10

# Συνδύασε motor alarm + position fault για κάθε joint
loadrt or2 count=8
addf or2.0 servo-thread
addf or2.1 servo-thread
addf or2.2 servo-thread
# ... (or2.3 - or2.7)

# Joint 0: Motor alarm OR Position fault = Any fault
net j0-motor-alarm    => or2.0.in0
net j0-pos-fault      => or2.0.in1
net j0-any-fault      <= or2.0.out

# Επανάλαβε για joints 1-7
```

### Γ. Trigger E-Stop σε Alarm:
```hal
# E-Stop Latch
loadrt estop_latch count=1
addf estop-latch.0 servo-thread

# Any motor alarm triggers E-stop
net j0-any-fault      => estop-latch.0.fault-in
net j1-any-fault      => estop-latch.0.fault-in
# ... (OR όλα τα faults)

# E-stop output
net estop-out         estop-latch.0.ok-out    => iocontrol.0.emc-enable-in
```

### Δ. Εναλλακτικά: Pause Αντί για E-Stop (Soft Warning):
```hal
# Pause motion on alarm (αντί για E-stop)
net j0-motor-alarm    => motion.pause

# ή χρησιμοποίησε motion.analog-in-XX για display
net j0-motor-alarm    => motion.digital-in-16
```

---

## 7. Troubleshooting

### Πρόβλημα: Alarm πάντα active (bit πάντα 1)
**Αίτιο:**
- Λάθος `Invert` setting στο JSON
- **A+ συνδεδεμένο στο GPIO pin αντί για +3.3V** ← ΣΥΝΗΘΕΣΤΕΡΟ
- Καλώδιο A- δεν συνδεδεμένο (floating pin)
- Pullup disabled στο JSON
- Driver έχει alarm active (πραγματικό πρόβλημα!)

**Λύση:**
1. **ΕΛΕΓΞΕ ΤΗ ΣΥΝΔΕΣΗ:** A+ → +3.3V, A- → GPIO pin (όχι το αντίθετο!)
2. Έλεγξε με multimeter:
   - Μέτρησε τάση μεταξύ A+ και GND: Πρέπει ~3.3V
   - Μέτρησε τάση στο GPIO pin (A-): Πρέπει ~3.3V κανονικά, ~0V σε alarm
3. Δοκίμασε `Invert: false` αντί για `true` (αν το wiring είναι αντίστροφο)
4. Βεβαιώσου ότι `Pullup: true` στο JSON
5. Έλεγξε το LED στον servo driver (alarm indicator)

### Πρόβλημα: Alarm δεν ανιχνεύεται ποτέ
**Αίτιο:**
- Pullup disabled όταν χρειάζεται
- Λάθος pin στο JSON
- **A+ και A- αντιστροφή** (A+ στο GPIO, A- στο +3.3V)
- Driver alarm output δεν δουλεύει
- **Καλώδιο alarm κομμένο ή χαλασμένο**

**Λύση:**
1. **ΕΛΕΓΞΕ ΤΗ ΣΥΝΔΕΣΗ:** A+ → +3.3V (power), A- → GPIO (input)
2. Ενεργοποίησε `Pullup: true` για differential OC outputs
3. Βεβαιώσου ότι το pin είναι σωστό (schematic)
4. **Δοκιμή με jumper wire:**
   - Short το A- στο +3.3V temporarily → πρέπει να δεις alarm cleared (bit=1)
   - Short το A- στο GND → πρέπει να δεις alarm triggered (bit=0)
5. Έλεγξε continuity του καλωδίου alarm με multimeter

### Πρόβλημα: False alarms (θόρυβος)
**Αίτιο:**
- EMI από step/dir σήματα (PWM, high-speed switching)
- Μακρύ καλώδιο χωρίς shielding
- Servo power supply noise
- Καλώδιο alarm κοντά σε power cables

**Λύση:**
1. Χρησιμοποίησε **shielded twisted pair cable** για A+/A-
2. Συνδυάζει το shield στο GND μόνο στη μία άκρη (STM32 side)
3. **Μέθοδος 2**: Πρόσθεσε **RC filter** (100Ω + 100nF) στο GPIO pin
4. **Μέθοδος 2**: External pullup (4.7kΩ) αντί για internal
5. Routing: Κράτα τα alarm cables μακριά από:
   - Power cables (AC/DC motor power)
   - Step/Dir cables (high-frequency switching)
   - Spindle/VFD cables
6. Στο firmware: Αύξησε το debounce (αλλά το Alarm module έχει μόνο 3 samples hardware debounce)

### Πρόβλημα: Κάηκε το GPIO pin!
**Αίτιο:**
- **A+ συνδεδεμένο στο GPIO pin** αντί για +3.3V
- Servo alarm output είναι 5V ή 24V (δεν είναι 3.3V compatible)

**Λύση (Prevention):**
1. **ΠΑΝΤΑパラ:** A+ → +3.3V (power pin), A- → GPIO (input pin)
2. Αν το servo alarm είναι 5V/24V, χρησιμοποίησε:
   - Voltage divider (2kΩ + 1kΩ για 5V → 3.3V)
   - Logic level converter module
   - **ΜΗΝ** συνδέσεις απευθείας!
3. Αν το pin κάηκε:
   - Αντικατέστησε το MCU (αν δεν έχει protection diodes)
   - Χρησιμοποίησε διαφορετικό pin (αλλαγή στο JSON config)

### Πρόβλημα: Alarm delay (αργεί να ανιχνευτεί)
**Αίτιο:**
- Debouncing στο Alarm module (3 samples @ 40kHz = 75µs)
- Long cable → RC delay (capacitance)
- External RC filter (αν προσθέσατε)

**Λύση:**
- 75µs debounce είναι αποδεκτό για τα περισσότερα συστήματα
- Αν χρειάζεται πιο γρήγορη ανίχνευση, αφαίρεσε το external RC filter
- Μείωσε το capacitance (μικρότερο καλώδιο, καλύτερο shielding)

---

### Διαγνωστικά Commands:

**Με Serial Monitor (115200 baud):**
```
ALARM: Joint 0 alarm triggered! (Count: 1)
Joint 0 in ALARM state
```

**Με LinuxCNC HAL:**
```bash
# Watch alarm bit
halcmd show pin remora.input.16    # Joint 0 alarm

# Simulate alarm (για testing)
halcmd setp remora.input.16 1      # Force alarm ON (δεν δουλεύει, read-only)
```

**Με Multimeter:**
```
1. Κανονική λειτουργία:
   - A+ to GND: ~3.3V
   - A- to GND: ~3.3V
   - A+ to A-:  ~0V (OPEN circuit, high impedance)

2. Alarm active:
   - A+ to GND: ~3.3V
   - A- to GND: ~3.3V (pulled up via A+ short)
   - A+ to A-:  ~0V (SHORT circuit, low resistance < 1Ω)

3. Test with jumper:
   - Short A- to +3.3V → Alarm OFF (pin reads HIGH)
   - Short A- to GND → Alarm ON (pin reads LOW)
```

---

## 8. Παράδειγμα JSON Config για 8 Άξονες με Alarms

```json
{
  "Board": "XCORE407I",
  "Modules": [
    {
      "Thread": "Base",
      "Type": "Stepgen",
      "Joint Number": 0,
      "Step Pin": "PE_4",
      "Direction Pin": "PE_5",
      "Enable Pin": "PE_3"
    },
    {
      "Thread": "Base",
      "Type": "Encoder",
      "Joint Number": 0,
      "Channel A Pin": "PD_0",
      "Channel B Pin": "PD_1"
    },
    {
      "Thread": "Servo",
      "Type": "Position Monitor",
      "Joint Number": 0,
      "Error Threshold": 2.0
    },
    {
      "Thread": "Base",
      "Type": "Alarm",
      "Comment": "Joint 0 - X Axis Motor Alarm",
      "Joint Number": 0,
      "Pin": "PC_0",
      "Data Bit": 16,
      "Invert": true,
      "Pullup": true
    },
    
    // Επανάλαβε για Joints 1-7...
  ]
}
```

---

## 9. Σύγκριση: Alarm vs Position Monitor

| Feature | Motor Alarm (Hardware) | Position Monitor (Firmware) |
|---------|------------------------|------------------------------|
| **Πηγή σήματος** | Servo driver (εξωτερικό) | STM32 firmware (εσωτερικό) |
| **Τι ελέγχει** | Overcurrent, encoder error, thermal | Position error (stepgen vs encoder) |
| **Ταχύτητα ανίχνευσης** | Άμεση (hardware) | 1 kHz (servo thread) |
| **Debouncing** | 3 samples (125µs @ 40kHz) | 10 cycles (10ms @ 1kHz) |
| **Data bit** | 16-23 (txData.inputs) | 8-15 (txData.inputs) |
| **Χρήση** | Hardware safety | Software position verification |

**Συνιστάται:** Χρησιμοποίησε **και τα δύο** για μέγιστη ασφάλεια!

---

## 10. Τεστ Checklist

- [ ] **Hardware:** Alarm signals συνδεδεμένα σε σωστά pins (PC_0-PC_3, PF_6-PF_9)
- [ ] **JSON Config:** Alarm modules δηλωμένα για κάθε joint
- [ ] **Pullup/Invert:** Σωστά settings ανάλογα με τον τύπο output (NPN/PNP)
- [ ] **Serial Monitor:** Debug messages εμφανίζονται όταν trigger alarm
- [ ] **LinuxCNC HAL:** Bits 16-23 διαβάζονται σωστά (`remora.input.16-23`)
- [ ] **E-Stop:** HAL logic triggers E-stop όταν alarm active
- [ ] **Noise Immunity:** Δεν υπάρχουν false alarms κατά τη διάρκεια motion

---

## 11. Σύνοψη

✅ **Motor Alarm Module** ανιχνεύει hardware faults από servo drivers  
✅ **Active-Low (NPN)** είναι ο συνηθέστερος τύπος (χρειάζεται pullup + invert)  
✅ **Bits 16-23** στο `txData.inputs` για 8 άξονες  
✅ **LinuxCNC HAL** διαβάζει τα bits και μπορεί να trigger E-stop  
✅ **Συνδυασμός με Position Monitor** δίνει 2 επίπεδα ασφαλείας (hardware + firmware)  

---

**Created:** October 26, 2025  
**Version:** 1.0  
**Board:** XCore407i (STM32F407IGT6)  
**Firmware:** Remora Ethernet-based Closed-Loop System
