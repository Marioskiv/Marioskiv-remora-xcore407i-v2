# Wiring Diagram - Differential Alarm Signals (A+/A-) για 6-Axis Servo System

## Πλήρης Συνδεσμολογία για 8 Closed-Loop Servos

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          STM32F407IGT6 (XCore407i)                       │
│                                                                           │
│  Power Pins:                                                              │
│  ┌────────────────────────────────────────────────────────────────┐     │
│  │  +3.3V (P2-37) ─────────────┬─ Common for ALL A+ connections  │     │
│  │                              │                                  │     │
│  │  GND (P2-70)                 │                                  │     │
│  └──────────────────────────────┼──────────────────────────────────┘     │
│                                 │                                         │
│  GPIO Alarm Input Pins:         │                                         │
│  ┌──────────────────────────────┴──────────────────────────────────┐    │
│  │  PC_0 (P1-??) <─ Joint 0 (X) Alarm A-  [Pullup enabled]        │    │
│  │  PC_1 (P1-??) <─ Joint 1 (Y) Alarm A-  [Pullup enabled]        │    │
│  │  PC_2 (P1-??) <─ Joint 2 (Z) Alarm A-  [Pullup enabled]        │    │
│  │  PC_3 (P1-??) <─ Joint 3 (A) Alarm A-  [Pullup enabled]        │    │
│  │  PF_6 (P2-??) <─ Joint 4 (B) Alarm A-  [Pullup enabled]        │    │
│  │  PF_7 (P2-??) <─ Joint 5 (C) Alarm A-  [Pullup enabled]        │    │
│  │  PF_8 (P2-??) <─ Joint 6 (D) Alarm A-  [Pullup enabled]        │    │
│  │  PF_9 (P2-??) <─ Joint 7 (E) Alarm A-  [Pullup enabled]        │    │
│  └──────────────────────────────────────────────────────────────────┘    │
└───────────────────────────────────────────────────────────────────────────┘
                                 │
                                 │ +3.3V common
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
        │                        │                        │
┌───────▼──────┐        ┌───────▼──────┐        ┌───────▼──────┐
│  Servo 0 (X) │        │  Servo 1 (Y) │        │  Servo 2 (Z) │
│              │        │              │        │              │
│  Alarm A+ ───┼────────┤  Alarm A+ ───┼────────┤  Alarm A+ ───┼──> +3.3V
│              │        │              │        │              │
│  Alarm A- ───┼──┐     │  Alarm A- ───┼──┐     │  Alarm A- ───┼──┐
└──────────────┘  │     └──────────────┘  │     └──────────────┘  │
                  │                       │                       │
                  │                       │                       │
                  ▼                       ▼                       ▼
                PC_0                    PC_1                    PC_2
              (GPIO)                  (GPIO)                  (GPIO)

... (επανάλαβε για Servos 3-7 με PC_3, PF_6, PF_7, PF_8, PF_9)
```

---

## Λειτουργία Photoelectric Isolated Open Collector:

### Κανονική Λειτουργία (Δεν υπάρχει alarm):
```
Servo Internal:
    [Optocoupler LED OFF] ──> Transistor OPEN
                               (High impedance)

External Circuit:
    +3.3V ──[ Pullup ]──┬──> GPIO Pin (PC_0)
                        │
                    A- <──┘ (OPEN from A+)
                        │
    +3.3V ──────────> A+

Result: GPIO Pin = HIGH (3.3V via pullup)
        txData.inputs bit = 0 (no alarm, after invert)
```

### Alarm Active (Servo ανίχνευσε πρόβλημα):
```
Servo Internal:
    [Optocoupler LED ON] ──> Transistor CLOSED
                              (Short A+ to A-)

External Circuit:
    +3.3V ──[ Pullup ]──┬──> GPIO Pin (PC_0)
                        │
                    A- <──┘ (SHORTED to A+)
                        │
    +3.3V ──────────> A+

Result: GPIO Pin = LOW (0V, pulled to A+ via short)
        txData.inputs bit = 1 (alarm active, after invert)
```

---

## Equivalent Circuit (ανά servo):

```
STM32 Side:
                                    ┌─────────────┐
    +3.3V (power)                   │  Servo      │
         │                          │  Driver     │
         │                          │             │
         ├───────────────────────────> A+ (Alarm+)│
         │                          │             │
         │  [40kΩ Internal Pullup] │ [Opto LED]  │
         │         │                │    │        │
         │         ├────────────────> A- (Alarm-) │
         │         │                │             │
    GPIO Pin <─────┘                └─────────────┘
    (PC_0)

Κανονική: Opto OFF → A+/A- OPEN   → Pin reads HIGH (via pullup)
Alarm:    Opto ON  → A+/A- SHORT  → Pin reads LOW  (via A+ connection)
```

---

## Με External Pullup + RC Filter (για θόρυβο):

```
STM32 Side:
                                    ┌─────────────┐
    +3.3V (power)                   │  Servo      │
         │                          │  Driver     │
         │                          │             │
         ├───────────────────────────> A+ (Alarm+)│
         │                          │             │
         │  [4.7kΩ External Pullup]│ [Opto LED]  │
         │         │                │    │        │
         │         │                │    │        │
         │         ├──[ 100Ω ]──┬───> A- (Alarm-) │
         │         │            │   │             │
         │                   [100nF] └─────────────┘
         │                      │
    GPIO Pin <──────────────────┤
    (PC_0)                      │
                              GND

RC Filter: 100Ω + 100nF = ~16 kHz cutoff (φιλτράρει EMI, διατηρεί σήμα)
External Pullup: 4.7kΩ (πιο δυνατό από 40kΩ internal)
```

---

## Cable Routing Best Practices:

```
CNC Machine Layout:

┌────────────────────────────────────────────────────┐
│                                                    │
│  [STM32 Controller]                                │
│       │                                            │
│       │ Alarm Cables (Shielded Twisted Pair)      │
│       │                                            │
│       ├────┬────┬────┬────┬────┬────┬────┬────┐   │
│       │    │    │    │    │    │    │    │    │   │
│   [Srv0][Srv1][Srv2][Srv3][Srv4][Srv5][Srv6][Srv7]│
│    X    Y    Z    A    B    C    D    E           │
│                                                    │
└────────────────────────────────────────────────────┘

Cable Guidelines:
✅ Χρησιμοποίησε shielded twisted pair (STP) για A+/A-
✅ Συνδυάζει shield σε GND μόνο στη μία άκρη (STM32 side)
✅ Κράτα alarm cables μακριά από:
   ❌ Motor power cables (AC/DC high current)
   ❌ Step/Dir signal cables (PWM switching noise)
   ❌ VFD/Spindle cables (high frequency EMI)
✅ Route alarm cables μαζί με encoder cables (low voltage signals)
✅ Αν δυνατόν, χρησιμοποίησε dedicated conduit για signal cables
```

---

## Connection Checklist (για κάθε servo):

**Servo 0 (X-Axis) Example:**

- [ ] **1. Identify A+/A- wires** στο servo connector
      - Συνήθως: Κόκκινο/Πορτοκαλί = A+, Μαύρο/Μπλε = A-
      - Δες servo manual για pinout

- [ ] **2. Connect A+ to +3.3V**
      - STM32 pin: P2-37 (+3.3V)
      - Χρησιμοποίησε ferrule/crimped terminal για καλή επαφή
      - **ΣΗΜΑΝΤΙΚΟ:** A+ πάει σε power pin, ΟΧΙ σε GPIO!

- [ ] **3. Connect A- to PC_0**
      - STM32 pin: PC_0 (GPIO input)
      - Δες schematic για exact header pin number

- [ ] **4. JSON Config έχει:**
      ```json
      {
        "Type": "Alarm",
        "Pin": "PC_0",
        "Pullup": true,
        "Invert": true
      }
      ```

- [ ] **5. Test με multimeter:**
      - A+ to GND: ~3.3V ✅
      - A- to GND: ~3.3V (κανονικά), ~0V (σε alarm) ✅
      - A+ to A-: High impedance (κανονικά), <1Ω (alarm) ✅

- [ ] **6. Test με jumper wire:**
      - Short A- to GND → Alarm triggered (bit=1 in LinuxCNC)
      - Remove short → Alarm cleared (bit=0)

- [ ] **7. Test με πραγματικό alarm:**
      - Disconnect encoder cable από servo
      - Servo driver πρέπει να δείξει alarm LED
      - STM32 serial monitor: "ALARM: Joint 0 alarm triggered!"
      - LinuxCNC: remora.input.16 = TRUE

- [ ] **8. Επανάλαβε για Servos 1-7**

---

## Παράδειγμα Terminal Block Wiring:

```
Terminal Block (STM32 Side):

    [TB1] +3.3V ──┬── Wire to Servo 0 A+
                  ├── Wire to Servo 1 A+
                  ├── Wire to Servo 2 A+
                  ├── Wire to Servo 3 A+
                  ├── Wire to Servo 4 A+
                  ├── Wire to Servo 5 A+
                  ├── Wire to Servo 6 A+
                  └── Wire to Servo 7 A+

    [TB2] PC_0 ──── Wire to Servo 0 A-
    [TB3] PC_1 ──── Wire to Servo 1 A-
    [TB4] PC_2 ──── Wire to Servo 2 A-
    [TB5] PC_3 ──── Wire to Servo 3 A-
    [TB6] PF_6 ──── Wire to Servo 4 A-
    [TB7] PF_7 ──── Wire to Servo 5 A-
    [TB8] PF_8 ──── Wire to Servo 6 A-
    [TB9] PF_9 ──── Wire to Servo 7 A-

    [TB10] GND ──── Shield drain (χρησιμοποίησε μόνο αν υπάρχει θόρυβος)
```

---

## Troubleshooting με Multimeter:

### Test 1: Verify A+ Connection
```
Multimeter: DC Voltage Mode
    Red probe → Servo A+ wire
    Black probe → STM32 GND

Expected: ~3.3V
If not: Έλεγξε σύνδεση στο +3.3V power pin
```

### Test 2: Verify A- Connection (Κανονική λειτουργία)
```
Multimeter: DC Voltage Mode
    Red probe → Servo A- wire (ή GPIO pin PC_0)
    Black probe → STM32 GND

Expected: ~3.3V (pulled up)
If 0V: Pullup disabled ή A- shorted to GND
```

### Test 3: Verify A- Connection (Alarm active)
```
Multimeter: DC Voltage Mode
    Red probe → Servo A- wire (ή GPIO pin PC_0)
    Black probe → STM32 GND

Expected: ~0V (pulled low via A+ short)
If 3.3V: Alarm δεν ενεργοποιείται ή driver πρόβλημα
```

### Test 4: Verify Isolation (Continuity Test)
```
⚠️ POWER OFF BOTH SERVO AND STM32!

Multimeter: Continuity/Resistance Mode
    Probe 1 → Servo A+
    Probe 2 → Servo A-

Expected (Κανονική): OPEN (infinite resistance, OL)
Expected (Alarm):    SHORT (<1Ω) - αλλά αυτό δεν μπορείς να δεις χωρίς power!

⚠️ ΜΗΝ μετράς continuity ενώ το σύστημα είναι powered!
```

---

**Created:** October 26, 2025  
**Version:** 2.0 (Updated for Differential A+/A- Signals)  
**Board:** XCore407i (STM32F407IGT6)  
**Compatible with:** Industrial servo motors με photoelectric isolated alarm output
