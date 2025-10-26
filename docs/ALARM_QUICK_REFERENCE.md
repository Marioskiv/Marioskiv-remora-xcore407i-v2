# 📌 Quick Reference: Differential Alarm Wiring (A+/A-) - 6-Axis System

## ⚡ Το Σημαντικότερο (TL;DR)

**Το servo motor σου έχει 2 καλώδια alarm:**
- **A+ (Alarm+)** - Κόκκινο/Πορτοκαλί
- **A- (Alarm-)** - Μαύρο/Μπλε

### ✅ Σωστή Σύνδεση (ΠΑΝΤΑ):
```
A+ ──────> +3.3V (STM32 power, π.χ. P2-37)
A- ──────> GPIO Pin (π.χ. PC_0, με pullup)
```

### ❌ Λάθος Σύνδεση (ΜΗΝ ΤΟ ΚΑΝΕΙΣ):
```
A+ ──X──> GPIO Pin   ← ΘΑ ΚΑΨΕΙΣ ΤΟ MCU!
A- ──X──> GND        ← ΔΕΝ ΘΑ ΔΟΥΛΕΨΕΙ!
```

---

## 🔌 Pin Mapping (6 Άξονες - XYZABC)

| Servo | Axis | A+ Σύνδεση | A- Σύνδεση | JSON Pin | Bit |
|-------|------|------------|------------|----------|-----|
| 0 | X | +3.3V (shared) | PC_0 | "PC_0" | 16 |
| 1 | Y | +3.3V (shared) | PC_1 | "PC_1" | 17 |
| 2 | Z | +3.3V (shared) | PC_2 | "PC_2" | 18 |
| 3 | A | +3.3V (shared) | PC_3 | "PC_3" | 19 |
| 4 | B | +3.3V (shared) | PF_6 | "PF_6" | 20 |
| 5 | C | +3.3V (shared) | PF_7 | "PF_7" | 21 |

**+3.3V Location:** P2-37 ή P2-69 στο XCore407i header

---

## 📝 JSON Config Template

```json
{
  "Thread": "Base",
  "Type": "Alarm",
  "Comment": "Joint X - Axis Name Motor Alarm",
  "Joint Number": 0,
  "Pin": "PC_0",
  "Data Bit": 16,
  "Invert": true,      // ΠΑΝΤΑ true για A+/A-
  "Pullup": true       // ΠΑΝΤΑ true για A+/A-
}
```

**Αντιγραφή για όλους τους άξονες:** Άλλαξε μόνο το `Pin`, `Joint Number`, `Data Bit`, `Comment`

---

## 🔧 Πώς Λειτουργεί

### Κανονική Λειτουργία (Δεν υπάρχει alarm):
```
Servo: A+/A- OPEN (high impedance)
GPIO: Reads HIGH (3.3V via pullup)
Bit:  0 (μετά το invert)
```

### Alarm Active (Servo error):
```
Servo: A+/A- SHORT (optocoupler closed)
GPIO: Reads LOW (0V, pulled to A+)
Bit:  1 (μετά το invert)
```

---

## ✅ Wiring Checklist

**Για κάθε servo:**

- [ ] Βρες τα καλώδια A+/A- στο servo connector (δες manual)
- [ ] Σύνδεσε **όλα** τα A+ μαζί στο +3.3V (P2-37)
- [ ] Σύνδεσε κάθε A- σε ξεχωριστό GPIO pin (PC_0, PC_1, etc.)
- [ ] JSON config έχει `Pullup: true` και `Invert: true`
- [ ] Δοκίμασε με multimeter:
  - [ ] A+ to GND = ~3.3V ✅
  - [ ] A- to GND = ~3.3V (κανονικά), ~0V (alarm) ✅

---

## 🛠️ Quick Test με Multimeter

```
Power ON το σύστημα (servo και STM32)

1. Μέτρησε A+ to GND:
   Αναμενόμενο: ~3.3V
   Αν όχι: Έλεγξε σύνδεση στο +3.3V pin

2. Μέτρησε A- to GND (κανονικά):
   Αναμενόμενο: ~3.3V (via pullup)
   Αν 0V: Pullup disabled ή A- shorted

3. Trigger alarm (disconnect encoder):
   Μέτρησε A- to GND: Πρέπει ~0V
   Αν 3.3V: Alarm δεν δουλεύει

4. Check serial monitor (115200 baud):
   "ALARM: Joint X alarm triggered!"
```

---

## 🚨 Common Mistakes

| Λάθος | Συμπτώματα | Λύση |
|-------|------------|------|
| A+ → GPIO, A- → GND | Κάηκε το GPIO pin | Αντικατάστασε MCU, σύνδεσε σωστά |
| A+ → GPIO, A- → +3.3V | Alarm πάντα active | Αντίστρεψε A+/A- |
| Pullup = false | Alarm πάντα active | Set `Pullup: true` |
| Invert = false | Alarm inverted | Set `Invert: true` |
| Καλώδιο A- χωρίς shielding | False alarms | Χρησιμοποίησε shielded cable |

---

## 📚 Αναλυτικά Documents

- **ALARM_DIFFERENTIAL_WIRING.md** - Πλήρη wiring diagrams, circuit analysis
- **ALARM_WIRING_GUIDE.md** - Τύποι alarm signals, troubleshooting
- **PIN_ASSIGNMENT.md** - Πλήρης pin mapping για όλο το board

---

## 📞 Τι Alarms Ανιχνεύονται

Το servo driver θα στείλει alarm για:

- ✅ **Encoder Error** - Πρόβλημα στον encoder
- ✅ **Overcurrent** - Υπερφόρτωση motor
- ✅ **Position Error** - Servo δεν μπορεί να ακολουθήσει
- ✅ **Overvoltage/Undervoltage** - Πρόβλημα τροφοδοσίας
- ✅ **Overtemperature** - Υπερθέρμανση
- ✅ **Communication Error** - Απώλεια επικοινωνίας με motor

---

**Created:** October 26, 2025  
**Board:** XCore407i (STM32F407IGT6)  
**Servo Type:** Closed-loop με A+/A- photoelectric isolated alarm
