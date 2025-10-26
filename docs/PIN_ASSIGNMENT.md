# XCore407I Pin Assignment Guide (Remora)

## 1. Pin Port Overview
- 2x headers P1 και P2, καθένας 2x35 (70 pins), βήμα 2.0mm
- Κάθε pin αντιστοιχεί σε net (PAx, PBx, PCx, κλπ)
- Δες το schematic: `resources/schematic xcore407i/xcore407i_pin_headers.json`

## 2. Reserved Pins (ΜΗ χρησιμοποιήσιμα για motion)
- **Ethernet (RMII):** PG11, PG13, PG14, PC4, PC5, PA1 (REFCLK), PA7 (CRS_DV/RX_DV), PC1 (MDC), PA2 (MDIO)
  - Στο header: PG11=P1-51, PG13=P1-53, PG14=P1-54, PA1=P1-36. Τα PC4/PC5/PC1/PA7 συνήθως ΔΕΝ είναι εκτεθειμένα στα headers.
- **Power/GND:**
  - P2-2 (5V_IN), P2-39 (5V FS USB)
  - P2-37 και P2-69 (3.3V)
  - GND: P1-58, P2-1, P2-38, P2-40, P2-70
- **Special pins:** BOOT0 (P1-61), RESET (P2-29), VBAT (P2-8), VREF+ (P2-32)

## 3. Stepper Motors (Step/Dir)
| Axis | Step Pin | Dir Pin |
|------|----------|---------|
| X    | PA8      | PA9     |
| Y    | PB6      | PB7     |
| Z    | PC6      | PC7     |
| A    | PD12     | PD13    |

## 4. Encoders (A/B/Z)
Σημείωση: Χρησιμοποιούμε ΜΟΝΟ pins που εκτίθενται στα headers P1/P2. Π.χ. PB10/PB11/PB12 και PC4/PC5 ΔΕΝ είναι διαθέσιμα στο header αυτού του board.

| Axis | A Pin | B Pin | Z Pin |
|------|-------|-------|-------|
| X    | PC8   | PC9   |       |
| Y    | PC11  | PC12  |       |
| Z    | PD2   | PD3   |       |

## 5. Limit Switches / Home Sensors
| Axis | Min Pin | Max Pin |
|------|---------|---------|
| X    | PA0     | PA1     |
| Y    | PB0     | PB1     |
| Z    | PC0     | PC1     |
| A    | PD0     | PD1     |

## 6. Motor Alarm Inputs (Closed-Loop Servo)
**Για closed-loop servo motors που έχουν differential alarm output (A+, A-) με photoelectric isolation:**

| Axis | A+ Connection | A- → Alarm Pin | Data Bit | Χρήση |
|------|---------------|----------------|----------|-------|
| X (Joint 0) | **+3.3V (P2-37)** | **PC_0** | 16 | Servo driver alarm (A+/A- differential) |
| Y (Joint 1) | **+3.3V (shared)** | **PC_1** | 17 | Servo driver alarm |
| Z (Joint 2) | **+3.3V (shared)** | **PC_2** | 18 | Servo driver alarm |
| A (Joint 3) | **+3.3V (shared)** | **PC_3** | 19 | Servo driver alarm |
| B (Joint 4) | **+3.3V (shared)** | **PF_6** | 20 | Servo driver alarm |
| C (Joint 5) | **+3.3V (shared)** | **PF_7** | 21 | Servo driver alarm |
| D (Joint 6) | **+3.3V (shared)** | **PF_8** | 22 | Servo driver alarm |
| E (Joint 7) | **+3.3V (shared)** | **PF_9** | 23 | Servo driver alarm |

**ΣΗΜΑΝΤΙΚΟ για Differential Alarm (A+/A-):**
- **A+ (Alarm+)**: Όλα τα servos συνδέονται στο **+3.3V** (shared, common connection)
- **A- (Alarm-)**: Κάθε servo συνδέεται σε ξεχωριστό **GPIO pin** (PC_0, PC_1, etc.)
- **Pullup**: ΠΑΝΤΑ enabled (`Pullup: true` στο JSON)
- **Invert**: ΠΑΝΤΑ true (`Invert: true` γιατί είναι active-low)

**Σύνδεση Hardware:**
```
Servo Motor (Photoelectric Isolated, Open Collector):
    Alarm A+ ──────────────> +3.3V (STM32 power pin P2-37)
    Alarm A- ──────────────> GPIO Pin (π.χ. PC_0 με pullup)

Λειτουργία:
    Κανονική: A+/A- OPEN   → GPIO = HIGH (3.3V via pullup)
    Alarm:    A+/A- SHORT  → GPIO = LOW  (0V via A+ connection)
```

**JSON Configuration:**
```json
{
  "Thread": "Base",
  "Type": "Alarm",
  "Comment": "Joint 0 - X Axis Motor Alarm (A+/A- Differential)",
  "Joint Number": 0,
  "Pin": "PC_0",         // A- connects here (GPIO input)
  "Data Bit": 16,
  "Invert": true,        // Active-low: alarm when SHORT (LOW)
  "Pullup": true         // REQUIRED for open collector output
}
```

**Σημειώσεις:**
- Τα alarm bits (16-23) στέλνονται στο LinuxCNC μέσω `txData.inputs`
- Συνδυάζονται με Position Monitor faults (bits 8-15) για πλήρη ασφάλεια
- **Photoelectric isolation** στο servo driver παρέχει galvanic isolation
- Δες **ALARM_DIFFERENTIAL_WIRING.md** για αναλυτικά wiring diagrams

## 7. Outputs (CNC)
- Γενικά CNC: Χωρίς θερμαντήρες/ανεμιστήρες. Μπορείς να ορίσεις ψηφιακές εξόδους (π.χ. relay) σε διαθέσιμα pins, όπως PE0, PE1, PD8–PD15, κλπ.

## 7. ADC Inputs (πληροφοριακά)
- Για CNC ΔΕΝ χρειάζονται. Αν τα χρησιμοποιήσεις μελλοντικά, προτίμησε καθαρά ADC pins (π.χ. PA0/PA1) που δεν συγκρούονται με limits.

## 8. General Inputs/Outputs
- Οποιοδήποτε ελεύθερο pin εκτός reserved μπορεί να χρησιμοποιηθεί για inputs/outputs.

## 9. Πώς να συνδέσεις τα καλώδια
- Βρες το net στο schematic (π.χ. PA8) και το αντίστοιχο header pin (π.χ. Pin 11).
- Δες το παραπάνω mapping για κάθε λειτουργία.
- Στο JSON config του Remora, δήλωσε το net name (π.χ. "step_pin": "PA8").
- Απέφυγε τα reserved pins!

## 10. Παράδειγμα JSON για 4 άξονες
```json
{
  "stepgen": [
    { "step_pin": "PA8",  "dir_pin": "PA9"  },
    { "step_pin": "PB6",  "dir_pin": "PB7"  },
    { "step_pin": "PC6",  "dir_pin": "PC7"  },
    { "step_pin": "PD12", "dir_pin": "PD13" }
  ],
  "encoder": [
    { "A_pin": "PC11", "B_pin": "PC12" },
    { "A_pin": "PD2",  "B_pin": "PD3"  },
    { "A_pin": "PC8",  "B_pin": "PC9"  }
  ],
  "limit": [
    { "min": "PD0", "max": "PD1" },
    { "min": "PE2", "max": "PE3" },
    { "min": "PE4", "max": "PE5" },
    { "min": "PE6", "max": "PE7" }
  ]
 }
```

---
**Για οποιαδήποτε λειτουργία, διάλεξε ελεύθερο pin από το header εκτός των reserved.**

---

## Σημεία προσοχής

- **Διπλή χρήση pins:**
  - Π.χ. PA0/PA1 έχουν δηλωθεί και ως limit switches και ως ADC inputs (thermistors).
  - **Δεν μπορείς να τα χρησιμοποιήσεις ταυτόχρονα!**
  - Διάλεξε αν θα είναι για limits ή για ADC/thermistor και άλλαξε το mapping ανάλογα με τις ανάγκες σου.

- **Enable pins για stepper drivers:**
  - Αν οι drivers σου χρειάζονται enable, δήλωσε `enable_pin` στο JSON (π.χ. PE0, PE1 ή άλλο ελεύθερο pin).
  - Παράδειγμα:
    ```json
    { "step_pin": "PA8", "dir_pin": "PA9", "enable_pin": "PE0" }
    ```

- **Z channel στους encoders:**
  - Αν δεν χρησιμοποιείς index (Z), μπορείς να αφήσεις το πεδίο κενό ή να το αφαιρέσεις από το JSON.
  - Παράδειγμα χωρίς Z:
    ```json
    { "A_pin": "PB10", "B_pin": "PB11" }
    ```

- **Γενικά:**
  - **Ποτέ μην χρησιμοποιείς το ίδιο pin για δύο λειτουργίες ταυτόχρονα!**
  - **Απέφυγε τα reserved pins (Ethernet, Power, ειδικά MCU pins).**
  - **Ενημέρωσε το JSON config και το wiring σου σύμφωνα με τις παραπάνω οδηγίες.**

---

## Διαθέσιμα pins για Step/Dir/Enable (έως 6 motors)

**Χρησιμοποίησε οποιοδήποτε από τα παρακάτω pins για κάθε άξονα, εκτός των reserved:**

### Step/Dir/Enable Pins (σύμφωνα με τα P1/P2 headers)
- P1: PA8, PA9, PA10, PA11, PA12, PA15
- P1: PB3, PB4, PB6, PB7, PB8, PB9, PB14, PB15 (Σημ.: PB10/PB11 δεν εκτίθενται)
- P1: PC6, PC7, PC8, PC9, PC10, PC11, PC12
- P1: PD0–PD15
- P1: PE0, PE1
- P2: PE2–PE6, PE7–PE15, PF0–PF10, PF11–PF15, PG0, PG1
- Απέφυγε RMII pins: PG11, PG13, PG14, PC4, PC5, PA1, PC1, PA2

### Reserved pins (μη διαθέσιμα):
- PG11, PG13, PG14, PC4, PC5, PA1 (REFCLK), PA7 (CRS_DV), PC1 (MDC), PA2 (MDIO)
- Όλες οι τροφοδοσίες (5V_IN, 3.3V) και GND (δες πιο πάνω)
- BOOT0 (P1-61), RESET (P2-29), VBAT (P2-8), VREF+ (P2-32)

---


## UART & ST-LINK Debug σημείωση

- Το firmware από προεπιλογή χρησιμοποιεί USART2 (PA2 TX, PA3 RX) για debug. Αυτά ΔΕΝ είναι προσβάσιμα στο P1/P2 header.
- Προτεινόμενο: Χρησιμοποίησε UART4 (PC10=TX, PC11=RX) στο header και όρισε build flag `-DUART_PORT=UART4` στο `platformio.ini` για να βλέπεις debug.
- Σύνδεση UART4: PC10 (TX πλακέτας) → RX adapter, PC11 (RX πλακέτας) → TX adapter, GND→GND. Baud 115200.

- **ST-LINK USART3 (PD8/PD9):**
  - Κάποια boards έχουν ST-LINK virtual COM port συνδεδεμένο στα PD8 (TX) και PD9 (RX).
  - Αν ενεργοποιήσεις USART3, μπορείς να δεις debug μέσω USB (ST-LINK VCP). Προσοχή: PD8/PD9 χρησιμοποιούνται και ως GPIO/enable pins αν δεν ενεργοποιηθεί USART3.
  - Για να χρησιμοποιήσεις ST-LINK USB serial, άλλαξε το build flag σε `-D UART_PORT=USART3` και σύνδεσε το USB του ST-LINK στον υπολογιστή σου. Άνοιξε serial monitor στην αντίστοιχη COM port (115200 baud).

- **USB Serial (monitor_port):**
  - Το PlatformIO monitor_port είναι συνήθως το COM port του USB-UART adapter ή του ST-LINK VCP.
  - Για debug μέσω UART4, χρησιμοποίησε το USB-UART adapter (PC10/PC11).
  - Για debug μέσω ST-LINK, χρησιμοποίησε το ST-LINK USB (PD8/PD9).

---

## Παράδειγμα Mapping για 8 άξονες (Closed-Loop με Alarm)

### Stepper Motors (Step/Dir/Enable)
| Axis | Step Pin | Dir Pin | Enable Pin |
|------|----------|---------|------------|
| X (J0) | PE_4   | PE_5    | PE_3       |
| Y (J1) | PE_0   | PE_1    | PE_6       |
| Z (J2) | PB_4   | PB_5    | PB_9       |
| A (J3) | PB_6   | PB_7    | PB_8       |
| B (J4) | PC_6   | PC_7    | PC_8       |
| C (J5) | PD_12  | PD_13   | PD_14      |
| D (J6) | PD_8   | PD_9    | PD_10      |
| E (J7) | PF_0   | PF_1    | PF_2       |

### Encoders (A/B channels)
| Axis | A Pin  | B Pin  |
|------|--------|--------|
| X (J0) | PD_0 | PD_1   |
| Y (J1) | PD_2 | PD_3   |
| Z (J2) | PD_4 | PD_5   |
| A (J3) | PD_6 | PD_7   |
| B (J4) | PC_9 | PC_10  |
| C (J5) | PC_11| PC_12  |
| D (J6) | PG_2 | PG_3   |
| E (J7) | PG_4 | PG_5   |

### Motor Alarms (Servo Driver Fault Signals)
| Axis | Alarm Pin | Data Bit |
|------|-----------|----------|
| X (J0) | PC_0    | 16       |
| Y (J1) | PC_1    | 17       |
| Z (J2) | PC_2    | 18       |
| A (J3) | PC_3    | 19       |
| B (J4) | PF_6    | 20       |
| C (J5) | PF_7    | 21       |
| D (J6) | PF_8    | 22       |
| E (J7) | PF_9    | 23       |

### Endstops/Limit Switches
| Axis | Endstop Pin | Data Bit |
|------|-------------|----------|
| X (J0) | PA_0      | 0        |
| Y (J1) | PA_1      | 1        |
| Z (J2) | PA_2      | 2        |
| A (J3) | PA_3      | 3        |
| B (J4) | PB_0      | 4        |
| C (J5) | PB_1      | 5        |
| D (J6) | PB_2      | 6        |
| E (J7) | PB_3      | 7        |

---

## Παράδειγμα Mapping για 6 motors

| Axis | Step Pin | Dir Pin | Enable Pin |
|------|----------|---------|------------|
| X    | PA8      | PA9     | PE0        |
| Y    | PB6      | PB7     | PE1        |
| Z    | PC6      | PC7     | PD8        |
| A    | PD12     | PD13    | PD14       |
| B    | PB8      | PB9     | PD15       |
| C    | PD5      | PD6     | PD7        |

---

## JSON Παράδειγμα για 6 motors
```json
{
  "stepgen": [
    { "step_pin": "PA8",  "dir_pin": "PA9",  "enable_pin": "PD8"  },   // X
    { "step_pin": "PB6",  "dir_pin": "PB7",  "enable_pin": "PD14" },   // Y
    { "step_pin": "PC6",  "dir_pin": "PC7",  "enable_pin": "PD15" },   // Z
    { "step_pin": "PD12", "dir_pin": "PD13", "enable_pin": "PE0"  },   // A
    { "step_pin": "PB8",  "dir_pin": "PB9",  "enable_pin": "PE1"  },   // B
    { "step_pin": "PD5",  "dir_pin": "PD6",  "enable_pin": "PD7"  }    // C
  ],
  "encoder": [
    { "A_pin": "PC8",  "B_pin": "PC9"  },   // X
    { "A_pin": "PC11", "B_pin": "PC12" },   // Y
    { "A_pin": "PD2",  "B_pin": "PD3"  },   // Z
    { "A_pin": "PH13", "B_pin": "PH14" },   // A
    { "A_pin": "PI0",  "B_pin": "PI1"  },   // B
    { "A_pin": "PI2",  "B_pin": "PI3"  }    // C
  ],
  "limit": [
    { "min": "PD0",  "max": "PD1"  },       // X
    { "min": "PE2",  "max": "PE3"  },       // Y
    { "min": "PE4",  "max": "PE5"  },       // Z
    { "min": "PE6",  "max": "PE7"  },       // A
    { "min": "PE8",  "max": "PE9"  },       // B
    { "min": "PE10", "max": "PE11" }        // C
  ]
}
```

---

## Πλήρης λίστα pins για κάθε λειτουργία

- **Stepper motors:** Step/Dir/Enable: PAx, PBx, PCx, PDx, PEx (εκτός reserved)
- **Encoders:** A/B/Z: PAx, PBx, PCx, PDx, PEx (εκτός reserved, και μόνο όσα εκτίθενται στα P1/P2)
- **Limit switches:** PAx, PBx, PCx, PDx, PEx (εκτός reserved)
- **Heaters/Fans/Outputs:** PA5, PB8, PE0, PE1, κλπ
- **ADC inputs:** PA0–PA7, PB0–PB1, PC0–PC5

Για κάθε λειτουργία, διάλεξε διαφορετικό pin για κάθε σήμα και **μην χρησιμοποιείς το ίδιο pin για δύο λειτουργίες ταυτόχρονα!**

---

## Handwheel (MPG), Extra Encoders, Probe, General Inputs

- **Handwheel/MPG:** Συνήθως είναι encoder (A/B/Z). Διάλεξε οποιοδήποτε ελεύθερο pin εκτός reserved για τα σήματα A/B/Z.
- **Extra encoders:** Μπορείς να προσθέσεις επιπλέον encoders για άξονες ή handwheel, αρκεί να δηλώσεις διαφορετικά pins για κάθε σήμα.
- **Probe, Home, E-stop, κλπ:** Οποιοδήποτε ελεύθερο pin μπορεί να χρησιμοποιηθεί ως ψηφιακή είσοδος για probe, home, emergency stop, κλπ.

### Παράδειγμα JSON για Handwheel/MPG encoder και Probe
```json
{
  "encoder": [
    { "A_pin": "PC11", "B_pin": "PC12" },
    { "A_pin": "PD2",  "B_pin": "PD3"  },
    { "A_pin": "PC8",  "B_pin": "PC9"  },
    { "A_pin": "PA10", "B_pin": "PA11" } // Handwheel/MPG (χωρίς Z)
  ],
  "probe": "PE12"
}
```

---

## Σημείωση

- Για κάθε λειτουργία (stepper, encoder, limit, handwheel, probe, κλπ), διάλεξε διαφορετικό pin για κάθε σήμα.
- **Μην χρησιμοποιείς το ίδιο pin για δύο λειτουργίες ταυτόχρονα!**
- **Απέφυγε τα reserved pins.**

---
