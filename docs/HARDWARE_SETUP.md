# 🔧 Hardware Setup Guide

Panduan lengkap setup hardware untuk Sistem Pemilah Sampah Cerdas.

## 📋 Daftar Komponen

### Main Components

| Komponen | Qty | Spesifikasi | Harga (est.) |
|----------|-----|-------------|--------------|
| ESP32 DevKit V1 | 1 | 240MHz, WiFi, BT | Rp 60.000 |
| ESP32-CAM | 1 | 2MP Camera, WiFi | Rp 45.000 |
| Servo SG90 | 3 | 0-180°, 5V | Rp 15.000 x3 |
| HC-SR04 | 1 | Ultrasonic sensor | Rp 10.000 |
| LED 5mm | 3 | Red, Green, Yellow | Rp 500 x3 |
| Resistor 220Ω | 3 | 1/4W | Rp 100 x3 |
| Breadboard | 1 | 830 points | Rp 15.000 |
| Jumper Wires | 1 set | M-M, M-F | Rp 10.000 |
| Power Supply | 1 | 5V 3A | Rp 25.000 |

**Total**: ~Rp 200.000

### Optional Components

- LCD I2C 16x2: Display info (Rp 30.000)
- Buzzer: Audio feedback (Rp 2.000)
- Push Button: Manual control (Rp 500)

## 🔌 Wiring Diagram

### ESP32 Main Controller

```
ESP32 DevKit V1
┌────────────────┐
│                │
│  GPIO 12  ──────> Servo Organik (Signal)
│  GPIO 13  ──────> Servo Anorganik (Signal)
│  GPIO 14  ──────> Servo B3 (Signal)
│                │
│  GPIO 26  ──────> HC-SR04 Trig
│  GPIO 27  ──────> HC-SR04 Echo
│                │
│  GPIO 32  ──────> LED Organik (+)
│  GPIO 33  ──────> LED Anorganik (+)
│  GPIO 25  ──────> LED B3 (+)
│                │
│  GND      ──────> GND (Common)
│  5V       ──────> 5V (Power)
│                │
└────────────────┘
```

### Detail Connections

#### Servo Motors

**Setiap Servo (x3):**
```
Servo Pin    →  ESP32 Pin
─────────────────────────
Brown (GND)  →  GND
Red (VCC)    →  5V (External)
Orange (SIG) →  GPIO 12/13/14
```

⚠️ **PENTING**: Servo butuh power terpisah!
- Jangan power dari ESP32 (max 500mA)
- Gunakan power supply 5V 3A external
- **Sambungkan GND** ESP32 dengan power supply

#### Ultrasonic Sensor (HC-SR04)

```
HC-SR04 Pin  →  ESP32 Pin
─────────────────────────
VCC          →  5V
Trig         →  GPIO 26
Echo         →  GPIO 27
GND          →  GND
```

#### LED Indicators

**Setiap LED (x3):**
```
Component         ESP32 Pin
────────────────────────────
LED Anode (+)  →  GPIO 32/33/25
Resistor 220Ω  →  (in series)
LED Cathode (-) →  GND
```

### Power Distribution

```
Power Supply 5V 3A
├── ESP32 DevKit (VIN)
├── 3x Servo Motors (VCC)
└── GND (Common Ground)
```

## 🔨 Assembly Steps

### Step 1: Breadboard Layout

1. **Letakkan komponen di breadboard:**
   ```
   [ESP32]  [Servo1]  [Servo2]  [Servo3]
   
   [HC-SR04]  [LED1]  [LED2]  [LED3]
   ```

2. **Power rails:**
   - Top rail: 5V
   - Bottom rail: GND

### Step 2: Connect Servos

1. **Servo Organik** (GPIO 12)
   - Brown → GND rail
   - Red → 5V rail (external)
   - Orange → GPIO 12

2. **Servo Anorganik** (GPIO 13)
   - Brown → GND rail
   - Red → 5V rail (external)
   - Orange → GPIO 13

3. **Servo B3** (GPIO 14)
   - Brown → GND rail
   - Red → 5V rail (external)
   - Orange → GPIO 14

### Step 3: Connect Ultrasonic

1. VCC → 5V rail
2. Trig → GPIO 26
3. Echo → GPIO 27
4. GND → GND rail

### Step 4: Connect LEDs

**Untuk setiap LED:**

1. **LED Organik (Green):**
   - Anode (+) → Resistor 220Ω → GPIO 32
   - Cathode (-) → GND

2. **LED Anorganik (Yellow):**
   - Anode (+) → Resistor 220Ω → GPIO 33
   - Cathode (-) → GND

3. **LED B3 (Red):**
   - Anode (+) → Resistor 220Ω → GPIO 25
   - Cathode (-) → GND

### Step 5: Power Connections

1. **External Power Supply:**
   - (+) → Breadboard 5V rail
   - (-) → Breadboard GND rail

2. **ESP32 Power:**
   - VIN → Breadboard 5V rail
   - GND → Breadboard GND rail

⚠️ **CRITICAL**: All GND must be connected!

## 🧪 Testing

### Test 1: Power On

1. Connect power supply
2. Check LED power indicator on ESP32
3. Upload test code:

```cpp
void setup() {
  Serial.begin(115200);
  Serial.println("Power OK!");
}

void loop() {
  delay(1000);
}
```

### Test 2: Servo Test

Upload `esp32_main.ino` and check:

```cpp
// Servo test akan berjalan otomatis di setup()
// Semua servo akan bergerak 0° → 90° → 0°
```

**Expected:**
- ✅ Servo 1 (Organik) bergerak
- ✅ Servo 2 (Anorganik) bergerak
- ✅ Servo 3 (B3) bergerak

### Test 3: LED Test

LED akan blink 3x saat startup.

**Expected:**
- ✅ LED hijau (Organik) blink
- ✅ LED kuning (Anorganik) blink
- ✅ LED merah (B3) blink

### Test 4: Ultrasonic Test

```cpp
void loop() {
  float distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(500);
}
```

**Expected:**
- ✅ Distance reading 2-400 cm
- ✅ Values change when moving hand

## 🔧 Troubleshooting

### Servo Tidak Bergerak

**Penyebab:**
- Power tidak cukup
- GND tidak terhubung
- Pin signal salah

**Solusi:**
1. Check power supply 5V 3A
2. Verify GND connections
3. Confirm GPIO pins

### ESP32 Restart Terus

**Penyebab:**
- Voltage drop dari servo
- Power supply lemah

**Solusi:**
1. Gunakan power supply 3A minimum
2. Tambah kapasitor 1000µF di servo VCC
3. Power servo terpisah dari ESP32

### LED Tidak Menyala

**Penyebab:**
- Resistor terlalu besar
- LED terbalik
- Pin GPIO salah

**Solusi:**
1. Check resistor 220Ω
2. Check polaritas LED (long leg = +)
3. Test dengan code manual:
   ```cpp
   digitalWrite(LED_ORGANIK, HIGH);
   ```

### Sensor Ultrasonik Error

**Penyebab:**
- Cable terlalu panjang
- Interference

**Solusi:**
1. Cable max 30cm
2. Jauh dari motor/servo
3. Add delay 50ms antar reading

## 📐 Mechanical Design (Optional)

### Struktur Pemilah

**Material:**
- Acrylic 3mm
- PVC pipe
- Kardus

**Dimensi:**
```
┌─────────────────────┐
│   Input Hopper      │
│   (Sensor di sini)  │
└──────┬──────────────┘
       │
   ┌───┴───┐
   │ Servo │ (3x untuk 3 bin)
   └───┬───┘
       │
┌──────┼──────┬───────┐
│  Org │ Anorg│  B3   │
│ Bin  │ Bin  │ Bin   │
└──────┴──────┴───────┘
```

### Servo Mechanism

**Pintu Pemilah:**
```
Servo di posisi 0°   → Semua tutup
Servo 1 ke 90°       → Buka bin Organik
Servo 2 ke 90°       → Buka bin Anorganik
Servo 3 ke 90°       → Buka bin B3
```

## 📱 ESP32-CAM Setup (Optional)

Jika menggunakan ESP32-CAM untuk capture:

### Wiring ESP32-CAM

```
ESP32-CAM  →  FTDI
──────────────────
5V         →  5V
GND        →  GND
U0R (RX)   →  TX
U0T (TX)   →  RX
IO0        →  GND (saat upload)
```

### Upload Code

1. Connect IO0 to GND
2. Upload `esp32cam_capture.ino`
3. Disconnect IO0 from GND
4. Reset ESP32-CAM

## 📋 Final Checklist

Before testing complete system:

- [ ] All components connected properly
- [ ] Power supply adequate (5V 3A)
- [ ] All GND connected (common ground)
- [ ] Servo test passed
- [ ] LED test passed
- [ ] Ultrasonic test passed
- [ ] Code uploaded to ESP32
- [ ] Serial monitor working (115200 baud)

## 🎓 Tips untuk UAS

**Simplified Version** (jika waktu terbatas):

1. **1 Servo only**: Test dengan 1 bin saja
2. **No ESP32-CAM**: Gunakan webcam laptop
3. **No Blynk**: Serial monitor cukup
4. **Focus**: Model detection + 1 servo working

**Demo Ready:**
- Sampah organik → LED hijau + Servo 1
- Sampah anorganik → LED kuning + Servo 2
- Record video demo!

## 🆘 Need Help?

- Check serial monitor output
- Use multimeter untuk cek voltage
- Test components one by one
- Simplify dulu, kompleks kemudian

Good luck! 🚀
