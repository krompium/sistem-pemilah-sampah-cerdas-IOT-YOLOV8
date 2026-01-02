# 🆘 Troubleshooting Guide - Sistem Pemilah Sampah Cerdas

Solusi untuk masalah umum yang mungkin terjadi.

---

## 📋 Table of Contents

1. [WiFi Connection Issues](#wifi-connection-issues)
2. [ESP32-CAM Problems](#esp32-cam-problems)
3. [Servo Issues](#servo-issues)
4. [Sensor Problems](#sensor-problems)
5. [Model Inference Issues](#model-inference-issues)
6. [Blynk Connection Problems](#blynk-connection-problems)
7. [Communication Errors](#communication-errors)
8. [Power Issues](#power-issues)
9. [Upload/Flashing Problems](#uploadflashing-problems)
10. [Performance Issues](#performance-issues)

---

## 1. WiFi Connection Issues

### ❌ Problem: ESP32 tidak connect ke WiFi

**Symptoms:**
```
📡 Connecting to WiFi...
....................
⚠️  WiFi connection FAILED!
```

**Possible Causes & Solutions:**

#### A. SSID/Password Salah
✅ **Solution:**
- Double check WiFi credentials di code
- Pastikan tidak ada typo
- Case-sensitive!

```cpp
const char* ssid = "NamaWiFiYangBenar";     // Cek lagi!
const char* password = "PasswordYangBenar";  // Pastikan benar!
```

#### B. WiFi 5GHz (ESP32 hanya support 2.4GHz)
✅ **Solution:**
- Gunakan WiFi 2.4GHz
- Atau buat separate 2.4GHz network di router

#### C. WiFi Channel Tidak Support
✅ **Solution:**
- Set router ke channel 1-11 (avoid 12-14)
- ESP32 tidak support channel >11 di beberapa region

#### D. Too Many Devices
✅ **Solution:**
- Router penuh (max devices)
- Disconnect device lain atau upgrade router

#### E. Signal Lemah
✅ **Solution:**
- Pindahkan ESP32 lebih dekat ke router
- Gunakan antena eksternal (jika ada)
- Cek signal strength di Serial Monitor:
```cpp
Serial.print("Signal: ");
Serial.print(WiFi.RSSI());  // Should be > -70 dBm
```

---

## 2. ESP32-CAM Problems

### ❌ Problem: ESP32-CAM tidak bisa upload code

**Symptoms:**
- Arduino IDE stuck di "Connecting..."
- Upload timeout
- "Failed to connect to ESP32"

**Solutions:**

#### A. GPIO 0 Tidak Ke GND
✅ **Solution:**
1. Sambungkan GPIO 0 ke GND **SEBELUM** upload
2. Click upload di Arduino IDE
3. **Setelah** "Connecting..." muncul, lepas GPIO 0
4. Wait upload selesai

#### B. Power Tidak Cukup
✅ **Solution:**
- **JANGAN** power dari USB FTDI!
- Gunakan external PSU 5V **3A minimum**
- Brownout = camera butuh banyak arus

#### C. Wrong Board Selection
✅ **Solution:**
- Tools → Board → **AI Thinker ESP32-CAM**
- Bukan "ESP32 Dev Module"

#### D. TX/RX Terbalik
✅ **Solution:**
```
ESP32-CAM U0T → FTDI RX
ESP32-CAM U0R → FTDI TX
```

### ❌ Problem: Camera tidak capture / brownout

**Symptoms:**
```
Brownout detector was triggered
Camera init failed
```

**Solutions:**

✅ **Gunakan PSU 5V 3A** (bukan 1A/2A)
✅ **Tambahkan capacitor 470μF-1000μF** di power line
✅ **Shorten power cables** (kurangi voltage drop)
✅ **Disable brownout detector** (last resort):
```cpp
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout
  // ...
}
```

### ❌ Problem: Motion detection tidak sensitif

**Solutions:**

✅ **Turunkan threshold** di code:
```cpp
int MOTION_THRESHOLD = 20;  // Lebih kecil = lebih sensitif
```

✅ **Adjust lighting** - motion detection butuh contrast
✅ **Check frame rate** - jangan terlalu lambat

---

## 3. Servo Issues

### ❌ Problem: Servo tidak gerak sama sekali

**Symptoms:**
- Servo diam / tidak respond
- Tidak ada suara servo

**Solutions:**

#### A. Power Issue
✅ **Check:**
- Servo dapat power 5V?
- GND connected?
- External power supply (jangan dari ESP32!)

#### B. Signal Wire Loose
✅ **Check:**
- Signal wire connected ke GPIO yang benar?
- GPIO 12 untuk penadah
- GPIO 13 untuk platform

#### C. Servo Rusak
✅ **Test servo** dengan code sederhana:
```cpp
#include <ESP32Servo.h>

Servo testServo;

void setup() {
  testServo.attach(12);
  testServo.write(90);
}

void loop() {
  delay(1000);
  testServo.write(0);
  delay(1000);
  testServo.write(180);
}
```

### ❌ Problem: Servo jitter / bergetar

**Symptoms:**
- Servo goyang-goyang di posisi
- Noise / humming sound

**Solutions:**

✅ **Add capacitor** (1000μF) parallel dengan servo power
✅ **Use dedicated servo power** (tidak share dengan ESP32)
✅ **Check angle** - jangan force servo ke posisi impossible
✅ **Reduce PWM frequency** jika perlu

### ❌ Problem: Servo gerak tapi tidak smooth

**Solutions:**

✅ **Slow down movement**:
```cpp
// Instead of instant:
servo.write(90);

// Do gradual:
for (int pos = 0; pos <= 90; pos++) {
  servo.write(pos);
  delay(15);
}
```

✅ **Add delay** setelah movement:
```cpp
servo.write(90);
delay(500);  // Let servo settle
```

---

## 4. Sensor Problems

### ❌ Problem: Ultrasonic sensor baca 0 atau nilai gila

**Symptoms:**
```
Distance: 0.00cm
Distance: 9999.00cm
```

**Solutions:**

#### A. Wiring Salah
✅ **Check:**
```
TRIG → GPIO (Output)
ECHO → GPIO (Input)  // JANGAN terbalik!
VCC  → 5V
GND  → GND
```

#### B. Objek Terlalu Dekat/Jauh
✅ **HC-SR04 range**: 2cm - 400cm
- Terlalu dekat (<2cm) → return 0
- Terlalu jauh (>400cm) → timeout

#### C. Timeout Issue
✅ **Increase timeout**:
```cpp
#define ULTRASONIC_TIMEOUT 30000  // 30ms (max ~5m)
```

#### D. Interference
✅ **Solutions:**
- Jangan mount 3 sensor terlalu dekat
- Beri jarak atau trigger sequential
- Avoid foam/soft materials (absorb ultrasonic)

### ❌ Problem: Reading tidak stabil

**Solutions:**

✅ **Add averaging**:
```cpp
float getDistanceAverage(int trig, int echo) {
  float sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += getDistance(trig, echo);
    delay(50);
  }
  return sum / 5.0;
}
```

✅ **Add filtering** (median filter)
✅ **Check mounting** - sensor harus stabil (tidak goyang)

---

## 5. Model Inference Issues

### ❌ Problem: Model tidak bisa load

**Symptoms:**
```
❌ Error: Model tidak ditemukan di models/best.pt
```

**Solutions:**

✅ **Check path**:
```bash
ls models/best.pt  # File ada?
```

✅ **Re-download** dari Google Colab
✅ **Check file size** - seharusnya ~50-100MB

### ❌ Problem: Inference sangat lambat (>5 detik)

**Solutions:**

✅ **Use GPU** (jika ada):
```python
import torch
print(torch.cuda.is_available())  # Should be True
```

✅ **Reduce image size**:
```python
IMAGE_SIZE = 320  # Instead of 640
```

✅ **Lower model size**:
- YOLOv8n (nano) - fastest
- YOLOv8s (small)
- YOLOv8m (medium) ← default
- YOLOv8l (large) - slowest

✅ **Close other apps** (free up CPU/GPU)

### ❌ Problem: Detection akurasi rendah

**Solutions:**

✅ **Lower confidence threshold**:
```python
CONFIDENCE_THRESHOLD = 0.50  # From 0.70
```

✅ **Re-train model** dengan lebih banyak data
✅ **Check dataset quality** - annotation benar?
✅ **Test different lighting** - model trained di lighting apa?

---

## 6. Blynk Connection Problems

### ❌ Problem: Blynk tidak connect

**Symptoms:**
```
☁️  Connecting to Blynk...
❌ Blynk connection FAILED!
```

**Solutions:**

#### A. Auth Token Salah
✅ **Verify** auth token:
- Copy ulang dari Blynk dashboard
- Pastikan 32 karakter
- No spaces!

#### B. WiFi Belum Connect
✅ **Connect WiFi dulu** sebelum Blynk
✅ **Check sequence**:
```cpp
connectWiFi();     // First!
connectBlynk();    // Then Blynk
```

#### C. Blynk Server Issue
✅ **Check** server status: https://status.blynk.cc
✅ **Try** different server (jika ada)

### ❌ Problem: Data tidak update di dashboard

**Solutions:**

✅ **Check virtual pins** match:
```cpp
// ESP32:
Blynk.virtualWrite(V0, value);

// Blynk Dashboard:
Datastream V0 configured?
```

✅ **Check device online** di Blynk dashboard
✅ **Force update**:
```cpp
Blynk.syncAll();  // Sync semua pins
```

---

## 7. Communication Errors

### ❌ Problem: ESP32-CAM tidak kirim foto ke laptop

**Symptoms:**
```
📤 Sending to laptop...
❌ Send failed! Error: -1
```

**Solutions:**

#### A. IP Address Salah
✅ **Verify** laptop IP:
```bash
# Windows:
ipconfig

# Linux/Mac:
ifconfig
```

✅ **Update** di ESP32-CAM code:
```cpp
const char* laptopIP = "192.168.1.XXX";  // IP laptop yang benar!
```

#### B. Firewall Block
✅ **Disable firewall** temporarily atau
✅ **Add exception** untuk port 5000

#### C. Flask Server Tidak Running
✅ **Check** laptop inference script running:
```bash
python inference/laptop_inference_dual.py
```

### ❌ Problem: Laptop tidak kirim hasil ke ESP32

**Solutions:**

✅ **Check ESP32 Main IP** di config
✅ **Test** dengan curl:
```bash
curl -X POST http://192.168.1.102/classify -d '{"class":0}'
```

✅ **Check** endpoint configured di ESP32

---

## 8. Power Issues

### ❌ Problem: ESP32 restart sendiri / brownout

**Symptoms:**
```
Brownout detector was triggered
rst:0x10 (RTCWDT_RTC_RESET)
```

**Solutions:**

✅ **Upgrade PSU** ke 5V 3A minimum
✅ **Add capacitor** 1000μF di power rail
✅ **Separate power** untuk servo
✅ **Shorten cables** (reduce voltage drop)
✅ **Check connections** - loose wire?

### ❌ Problem: Servo bergerak menyebabkan reset

**Solutions:**

✅ **NEVER power servo dari ESP32!**
✅ **Use external 5V PSU** untuk servo
✅ **Common ground** ESP32 + Servo power

---

## 9. Upload/Flashing Problems

### ❌ Problem: "Failed to connect to ESP32"

**Solutions:**

✅ **Press & hold BOOT button** saat upload
✅ **Check USB cable** - data cable, bukan charge-only
✅ **Install CP210x driver** (Google "CP2102 driver")
✅ **Try different USB port**
✅ **Lower upload speed**: Tools → Upload Speed → 115200

### ❌ Problem: "Sketch too large"

**Solutions:**

✅ **Partition scheme**: Tools → Partition Scheme → "Huge APP (3MB)"
✅ **Remove debug code** / comments
✅ **Optimize libraries**

---

## 10. Performance Issues

### ❌ Problem: System lambat / lag

**Solutions:**

✅ **Reduce Serial.print()** - banyak print = lambat
✅ **Optimize loop()** - jangan banyak delay
✅ **Use millis()** instead of delay
✅ **Check WiFi signal** - weak signal = slow

### ❌ Problem: Memory leak / heap overflow

**Symptoms:**
```
Guru Meditation Error: Core 1 panic'ed
```

**Solutions:**

✅ **Free unused memory**:
```cpp
esp_camera_fb_return(fb);  // Return frame buffer!
```

✅ **Check memory usage**:
```cpp
Serial.println(ESP.getFreeHeap());
```

✅ **Reduce buffer sizes** jika perlu

---

## 🔍 Debugging Tips

### Enable Debug Output:

```cpp
// ESP32 code:
#define DEBUG 1

#if DEBUG
  #define DEBUG_PRINT(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
#endif
```

### Python Debug:

```python
DEBUG_MODE = True
VERBOSE_LOGGING = True
```

### Serial Monitor Best Practices:

- Baud rate: **115200**
- Enable timestamp
- Save log to file untuk analysis
- Use Serial Plotter untuk sensor data

---

## 📞 Getting Help

Jika masih stuck:

1. **Check dokumentasi lain**:
   - `QUICK_START_GUIDE.md`
   - `HARDWARE_WIRING.md`
   - `CALIBRATION_GUIDE.md`

2. **Google error message** - often helpful!

3. **Check GitHub Issues** di repository

4. **Forum ESP32**:
   - https://esp32.com
   - https://forum.arduino.cc

5. **YOLOv8 Issues**:
   - https://github.com/ultralytics/ultralytics/issues

---

## ✅ Preventive Maintenance

Untuk avoid masalah di masa depan:

- [ ] **Regular cleaning** - dust affects sensors
- [ ] **Check connections** monthly
- [ ] **Backup config** sebelum changes
- [ ] **Document custom settings**
- [ ] **Test sebelum presentation!**
- [ ] **Charge backup power bank**
- [ ] **Prepare demo video** (jika WiFi bermasalah saat demo)

---

**Good luck! Semoga sistem berjalan lancar untuk UAS! 🚀**
