# 🔌 Hardware Wiring Guide - Sistem Pemilah Sampah Cerdas

Panduan lengkap koneksi hardware untuk sistem pemilah sampah otomatis.

---

## 📋 Component List

| No | Component | Qty | Specifications |
|----|-----------|-----|----------------|
| 1 | ESP32 DevKit V1 | 1 | Main controller |
| 2 | ESP32-CAM AI Thinker | 1 | Camera & motion detection |
| 3 | Servo Motor | 2 | SG90 atau MG996R |
| 4 | HC-SR04 Ultrasonic | 3 | Sensor jarak untuk bin full |
| 5 | LED 5mm | 5 | Hijau, Biru, Merah, Putih, Kuning |
| 6 | Resistor 220Ω | 5 | Untuk LED |
| 7 | Breadboard | 2 | 830 points |
| 8 | Jumper Wires | 40+ | Male-male, male-female |
| 9 | Power Supply | 1 | 5V 3A minimum |
| 10 | Buck Converter | 1 | 5V output (untuk ESP32-CAM) |

---

## 🎯 ESP32 Main Controller - Pin Connections

### Complete Pin Mapping Table:

| Component | Function | ESP32 Pin | Notes |
|-----------|----------|-----------|-------|
| **SERVO PENADAH** | Signal | GPIO 12 | PWM capable |
| | VCC | 5V | External power |
| | GND | GND | Common ground |
| **SERVO PLATFORM** | Signal | GPIO 13 | PWM capable |
| | VCC | 5V | External power |
| | GND | GND | Common ground |
| **ULTRASONIC ORGANIK** | TRIG | GPIO 26 | Output |
| | ECHO | GPIO 27 | Input |
| | VCC | 5V | - |
| | GND | GND | - |
| **ULTRASONIC ANORGANIK** | TRIG | GPIO 14 | Output |
| | ECHO | GPIO 25 | Input |
| | VCC | 5V | - |
| | GND | GND | - |
| **ULTRASONIC B3** | TRIG | GPIO 32 | Output |
| | ECHO | GPIO 33 | Input |
| | VCC | 5V | - |
| | GND | GND | - |
| **LED ORGANIK** | Anode (+) | GPIO 18 | Via 220Ω resistor |
| | Cathode (-) | GND | - |
| **LED ANORGANIK** | Anode (+) | GPIO 19 | Via 220Ω resistor |
| | Cathode (-) | GND | - |
| **LED B3** | Anode (+) | GPIO 21 | Via 220Ω resistor |
| | Cathode (-) | GND | - |
| **LED SYSTEM** | Anode (+) | GPIO 22 | Via 220Ω resistor |
| | Cathode (-) | GND | - |
| **LED ERROR** | Anode (+) | GPIO 23 | Via 220Ω resistor |
| | Cathode (-) | GND | - |

### Wiring Diagram (ASCII Art):

```
                    ESP32 DevKit V1
                   ┌──────────────┐
                   │              │
    Servo Penadah ─┤ GPIO 12      │
  Servo Platform  ─┤ GPIO 13      │
                   │              │
    TRIG Organik  ─┤ GPIO 26      │
    ECHO Organik  ─┤ GPIO 27      │
  TRIG Anorganik  ─┤ GPIO 14      │
  ECHO Anorganik  ─┤ GPIO 25      │
       TRIG B3    ─┤ GPIO 32      │
       ECHO B3    ─┤ GPIO 33      │
                   │              │
   LED Organik    ─┤ GPIO 18      │
 LED Anorganik    ─┤ GPIO 19      │
      LED B3      ─┤ GPIO 21      │
   LED System     ─┤ GPIO 22      │
    LED Error     ─┤ GPIO 23      │
                   │              │
          5V ──────┤ 5V           │
         GND ──────┤ GND          │
                   └──────────────┘
```

---

## 📷 ESP32-CAM - Connections

### ESP32-CAM Pinout:

```
           ESP32-CAM AI Thinker
          ┌──────────────────┐
          │   [CAMERA OV2640] │
          │                   │
      5V ─┤ 5V    GPIO 4 (LED)│─ Flash LED (built-in)
     GND ─┤ GND   GPIO 0      │─ Program mode (to GND for flash)
    U0R ─┤ U0R   GPIO 1       │
    U0T ─┤ U0T   Reset        │─ Reset button
          └──────────────────┘
```

### Programming Connection (untuk upload code):

| ESP32-CAM | FTDI/USB-Serial |
|-----------|-----------------|
| 5V | 5V |
| GND | GND |
| U0R | TX |
| U0T | RX |
| GPIO 0 | GND (saat upload) |

**PENTING**:
- Gunakan power supply 5V **3A minimum** untuk ESP32-CAM
- Jangan power dari USB (tidak cukup arus)
- GPIO 0 ke GND **hanya saat upload**, lepas setelah upload selesai

---

## ⚡ Power Distribution

### Power Requirements:

| Component | Voltage | Current | Notes |
|-----------|---------|---------|-------|
| ESP32 Main | 5V | ~500mA | Via USB atau pin 5V |
| ESP32-CAM | 5V | ~1.5A | **HARUS 3A PSU!** |
| Servo x2 | 5V | ~1A total | Tergantung load |
| Ultrasonic x3 | 5V | ~15mA | Kecil |
| LED x5 | 3.3V | ~100mA | Via resistor |
| **TOTAL** | **5V** | **~3A** | Minimal PSU 5V 3A |

### Recommended Power Setup:

```
    5V 3A Power Supply
           |
           |
    ┌──────┴────────┐
    │               │
    │               │
  ESP32          ESP32-CAM
  Main           (via Buck Converter)
    |
    |
  ┌─┴─┐
Servo  Servo
  1     2
```

**Tips**:
- Gunakan **external power** untuk servo (jangan dari ESP32)
- Pastikan **common ground** antara semua komponen
- Gunakan **capacitor 1000μF** parallel dengan servo untuk stability

---

## 🔧 Mechanical Assembly

### 1. Penadah Sementara Setup:

```
     ESP32-CAM (di atas, hadap ke bawah)
           |
     ┌─────┴─────┐
     │ PENADAH   │ ← Container sementara
     │ SEMENTARA │
     └───────────┘
           |
      Servo Buka/Tutup ← GPIO 12
           |
           ↓
     (sampah jatuh ke bin)
```

### 2. Platform Rotasi Setup:

```
                   Servo Platform (GPIO 13)
                          |
                          ↓
    ┌─────────────────────────────────┐
    │    PLATFORM PUTAR (360°)         │
    └─────────────────────────────────┘
           |         |          |
           ↓         ↓          ↓
       ┌────┐    ┌────┐    ┌────┐
       │BIN │    │BIN │    │BIN │
       │ 0° │    │90° │    │180°│
       │ORG │    │ANO │    │ B3 │
       └────┘    └────┘    └────┘
```

### 3. Sensor Ultrasonik Placement:

```
    [HC-SR04]  [HC-SR04]  [HC-SR04]
        ↓          ↓          ↓
     (mounted di atas bin)
        
    ┌────┐    ┌────┐    ┌────┐
    │    │    │    │    │    │
    │BIN │    │BIN │    │BIN │
    │ORG │    │ANO │    │ B3 │
    └────┘    └────┘    └────┘
    
    Sensor detect jarak ke permukaan sampah
```

---

## 🎨 LED Color Coding

| LED | Color | GPIO | Function |
|-----|-------|------|----------|
| 1 | 🟢 Green | 18 | Organik detected |
| 2 | 🔵 Blue | 19 | Anorganik detected |
| 3 | 🔴 Red | 21 | B3 detected |
| 4 | ⚪ White | 22 | System OK/Running |
| 5 | 🟡 Yellow | 23 | Error/Bin Full |

### LED Circuit (Each):

```
GPIO Pin ─┬─ 220Ω ─┬─ LED(+) ──┐
          │         │            │
          │         └─ LED(-)    │
          │                      │
          └──────────────────────┴─ GND
```

---

## ⚠️ Common Wiring Mistakes

### ❌ **Jangan Lakukan Ini**:

1. **Power ESP32-CAM dari USB**
   - USB tidak cukup arus → camera brownout
   - Gunakan PSU 5V 3A

2. **Tidak pakai resistor untuk LED**
   - LED bisa terbakar
   - GPIO bisa rusak

3. **Servo power dari ESP32**
   - ESP32 tidak kuat supply arus servo
   - Bisa restart/brownout

4. **Tidak common ground**
   - Signal tidak stabil
   - Random errors

5. **Kabel terlalu panjang**
   - Signal loss
   - Noise interference

### ✅ **Best Practices**:

1. **Gunakan external PSU untuk:**
   - Servo (5V)
   - ESP32-CAM (5V 3A)

2. **Common ground SEMUA komponen**

3. **Kabel pendek (<30cm)**
   - Especially untuk sensor & servo

4. **Test satu-satu component**
   - Jangan rakit semuanya sekaligus

5. **Label semua kabel**
   - Easier debugging

---

## 🧪 Testing Checklist

### Before Power On:
- [ ] Check all connections dengan multimeter
- [ ] Pastikan tidak ada short circuit
- [ ] Cek polaritas LED & power
- [ ] Pastikan ESP32-CAM dapat PSU dedicated

### After Power On:
- [ ] ESP32 Main power LED nyala
- [ ] ESP32-CAM power LED nyala (jangan reboot loop)
- [ ] LED System blink (program running)
- [ ] Serial Monitor output OK

### Component Tests:
- [ ] Servo Penadah smooth motion 0-90°
- [ ] Servo Platform smooth rotation 0-360°
- [ ] Ultrasonic baca jarak accurate (test dengan penggaris)
- [ ] All LEDs dapat nyala
- [ ] Camera ESP32-CAM capture OK

---

## 📸 Photo Documentation

**Tips**: Foto rangkaian Anda untuk dokumentasi UAS!

Foto yang perlu:
1. Top view (tampak atas) - semua komponen
2. ESP32 connections close-up
3. Servo mechanism
4. Sensor placement
5. Final assembled system

---

## 🆘 Troubleshooting

### ESP32-CAM tidak upload:
- Pastikan GPIO 0 ke GND saat flash
- Gunakan PSU 5V 3A (bukan USB)
- Press reset sebelum upload

### Servo tidak gerak:
- Cek power supply (5V, cukup arus?)
- Cek signal wire
- Test servo dengan sketch sederhana

### Sensor Ultrasonik baca 0:
- Cek koneksi TRIG/ECHO
- Jangan terlalu dekat objek (<2cm)
- Test dengan Serial print

### LED tidak nyala:
- Cek polaritas (anode/cathode)
- Cek resistor 220Ω
- Test dengan digitalWrite manual

---

**Next**: Setelah wiring selesai, lanjut ke `CALIBRATION_GUIDE.md` untuk kalibrasi servo & sensor.
