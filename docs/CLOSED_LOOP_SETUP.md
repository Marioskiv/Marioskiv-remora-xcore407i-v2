# 8-Axis Closed-Loop Configuration Guide

## Περιγραφή (Description)

Αυτό το configuration υποστηρίζει 8 άξονες (joints) με πλήρες closed-loop control, συμπεριλαμβανομένων:
- **8x Stepgen modules** - Για έλεγχο step/direction motors ή closed-loop servo drives
- **8x Encoder modules** - Για position feedback από quadrature encoders
- **8x Endstop modules** - Για homing και limit switches
- **8x Alarm modules** - Για motor error/alarm signals από closed-loop servo drives

This configuration supports 8 axes (joints) with full closed-loop control, including:
- **8x Stepgen modules** - For step/direction motor control or closed-loop servo drives
- **8x Encoder modules** - For position feedback from quadrature encoders
- **8x Endstop modules** - For homing and limit switches
- **8x Alarm modules** - For motor error/alarm signals from closed-loop servo drives

---

## Modules Ανά Άξονα (Per Axis)

Κάθε άξονας έχει 4 modules:

### 1. **Stepgen Module**
Στέλνει step/direction pulses στο servo drive.
- `Enable Pin` - Enable output για το motor drive
- `Step Pin` - Step pulse output
- `Direction Pin` - Direction output
- `Step Bit` - DDS accumulator bit (συνήθως 22)

### 2. **Encoder Module**
Διαβάζει την πραγματική θέση από τον encoder του motor.
- `Channel A Pin` - Encoder A phase input
- `Channel B Pin` - Encoder B phase input
- `Index Pin` (optional) - Encoder index/Z pulse για absolute reference

Το feedback στέλνεται στο LinuxCNC μέσω του `txData.jointFeedback[joint]` array.

### 3. **Endstop Module**
Διαβάζει limit/home switches.
- `Pin` - Digital input για το switch
- `Data Bit` - Bit position στο inputs word (0-15)
- `Invert` - Αντιστροφή λογικής (true για normally-closed switches)
- `Pullup` - Enable internal pullup resistor

### 4. **Alarm Module**
Διαβάζει alarm/error signals από το servo drive.
- `Pin` - Digital input για το alarm signal
- `Data Bit` - Bit position στο inputs word (8-15 για alarms)
- `Invert` - Αντιστροφή λογικής (true για active-low alarms)
- `Pullup` - Enable internal pullup resistor

Όταν ανιχνευθεί alarm, εμφανίζεται μήνυμα στο console και το bit ενεργοποιείται στο LinuxCNC.

---

## Pin Assignments

### Άξονας 0 (X)
- Step: PE_4, Dir: PE_5, Enable: PE_3
- Encoder: PD_0 (A), PD_1 (B)
- Endstop: PA_0 (bit 0)
- Alarm: PC_0 (bit 8)

### Άξονας 1 (Y)
- Step: PE_0, Dir: PE_1, Enable: PE_6
- Encoder: PD_2 (A), PD_3 (B)
- Endstop: PA_1 (bit 1)
- Alarm: PC_1 (bit 9)

### Άξονας 2 (Z)
- Step: PB_4, Dir: PB_5, Enable: PB_9
- Encoder: PD_4 (A), PD_5 (B)
- Endstop: PA_2 (bit 2)
- Alarm: PC_2 (bit 10)

### Άξονας 3 (A)
- Step: PD_12, Dir: PD_13, Enable: PB_8
- Encoder: PD_6 (A), PD_7 (B)
- Endstop: PA_3 (bit 3)
- Alarm: PC_3 (bit 11)

### Άξονας 4 (B)
- Step: PD_14, Dir: PD_15, Enable: PD_9
- Encoder: PE_7 (A), PE_8 (B)
- Endstop: PA_4 (bit 4)
- Alarm: PC_4 (bit 12)

### Άξονας 5 (C)
- Step: PG_2, Dir: PG_3, Enable: PD_10
- Encoder: PE_9 (A), PE_10 (B)
- Endstop: PA_5 (bit 5)
- Alarm: PC_5 (bit 13)

### Άξονας 6 (U)
- Step: PG_5, Dir: PG_6, Enable: PG_4
- Encoder: PE_11 (A), PE_12 (B)
- Endstop: PA_6 (bit 6)
- Alarm: PC_6 (bit 14)

### Άξονας 7 (V)
- Step: PG_8, Dir: PG_9, Enable: PG_7
- Encoder: PE_13 (A), PE_14 (B)
- Endstop: PA_7 (bit 7)
- Alarm: PC_7 (bit 15)

---

## Data Structure

### TX Data (από STM32 προς LinuxCNC)
```cpp
txData.jointFeedback[0-7]  // Encoder counts για κάθε άξονα
txData.inputs              // 16-bit word:
                           //   bits 0-7: Endstops
                           //   bits 8-15: Alarms
```

### RX Data (από LinuxCNC προς STM32)
```cpp
rxData.jointFreqCmd[0-7]   // Frequency commands για stepgen
rxData.jointEnable         // 8-bit enable mask για κάθε άξονα
```

---

## LinuxCNC HAL Configuration

Παράδειγμα HAL configuration για closed-loop control με PID:

```hal
loadrt remora-eth
loadrt pid names=j0pid,j1pid,j2pid,j3pid,j4pid,j5pid,j6pid,j7pid

# Joint 0 - Closed Loop with Encoder Feedback
setp remora.joint.0.scale       [JOINT_0]SCALE
setp remora.joint.0.encoder-scale [JOINT_0]ENCODER_SCALE

net j0enable      <= joint.0.amp-enable-out => remora.joint.0.enable
net j0enable                                  => j0pid.enable
net j0pos-fb      <= remora.joint.0.encoder-count => j0pid.feedback
net j0pos-fb                                  => joint.0.motor-pos-fb
net j0pos-cmd     <= joint.0.motor-pos-cmd   => j0pid.command
net j0pid-output  <= j0pid.output            => remora.joint.0.vel-cmd

# Endstop
net j0-home-sw    <= remora.input.0          => joint.0.home-sw-in
net j0-limit-sw   <= remora.input.0          => joint.0.neg-lim-sw-in
net j0-limit-sw   <= remora.input.0          => joint.0.pos-lim-sw-in

# Alarm - triggers E-stop in LinuxCNC
net j0-alarm      <= remora.input.8          => motion.digital-in-00
# You can connect this to estop or create custom logic

# Repeat για όλους τους άξονες...
```

---

## Servo Drive Wiring

### Typical Closed-Loop Servo Drive Connections:

1. **STEP+/STEP-** ← STM32 Step Pin (differential signaling recommended)
2. **DIR+/DIR-** ← STM32 Direction Pin
3. **ENA+/ENA-** ← STM32 Enable Pin
4. **ALARM** → STM32 Alarm Input (typically open-collector, needs pullup)
5. **ENCODER A+/A-** → STM32 Encoder Channel A
6. **ENCODER B+/B-** → STM32 Encoder Channel B
7. **ENCODER Z+/Z-** → STM32 Encoder Index (optional)

**Σημείωση**: Πολλά servo drives έχουν differential signals. Μπορεί να χρειαστείτε line receivers (π.χ. AM26C32) για conversion σε single-ended signals για τον STM32.

---

## Tuning PID Parameters

Για κάθε άξονα, ρυθμίστε τα εξής στο INI file:

```ini
[JOINT_0]
P = 30.0          # Proportional gain
I = 0.0           # Integral gain (συνήθως 0 για position loop)
D = 0.0           # Derivative gain
FF0 = 0.0         # Position feedforward
FF1 = 1.0         # Velocity feedforward (συνήθως 1.0)
FF2 = 0.0         # Acceleration feedforward
BIAS = 0.0        # Output bias
DEADBAND = 0.01   # Error deadband
FERROR = 5.0      # Following error limit
MIN_FERROR = 1.0  # Minimum following error
```

---

## Troubleshooting

### Encoder δεν λειτουργεί:
1. Ελέγξτε τις συνδέσεις A, B channels
2. Ελέγξτε το encoder scale στο LinuxCNC
3. Ελέγξτε αν το encoder count αλλάζει: `halscope` ή `halmeter`

### Alarm triggers συνεχώς:
1. Ελέγξτε το `Invert` setting (ίσως χρειάζεται true/false)
2. Ελέγξτε το servo drive manual για alarm signal type
3. Ελέγξτε αν υπάρχει alarm condition στο drive (LED indicators)

### Άξονας κινείται αντίστροφα:
1. Αλλάξτε το sign του SCALE στο INI file
2. Ή swap τα A/B encoder channels
3. Ή invert το Direction pin signal

### Following Error:
1. Αυξήστε το FF1 (velocity feedforward)
2. Μειώστε το P gain αν oscillates
3. Ελέγξτε το encoder scale (πρέπει να ταιριάζει με το step scale)

---

## Credits

Based on Remora firmware architecture.
Encoder algorithm from Paul Stoffregen's Encoder library.
Configuration inspired by Remora-OS6 closed-loop examples.

---

## Άδεια (License)

Αυτό το configuration και ο κώδικας είναι μέρος του Remora project.
Δείτε το LICENSE file για λεπτομέρειες.
