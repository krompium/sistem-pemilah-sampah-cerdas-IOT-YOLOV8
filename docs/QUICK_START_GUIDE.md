# 🚀 Quick Start Guide - Sistem Pemilah Sampah Cerdas

Panduan lengkap setup sistem dari awal sampai demo ready dalam **~90 menit**.

---

## 📋 Prerequisites

### Hardware yang Dibutuhkan:
- [x] 1x ESP32 DevKit V1
- [x] 1x ESP32-CAM AI Thinker
- [x] 2x Servo SG90/MG996R
- [x] 3x HC-SR04 Ultrasonic Sensor
- [x] 5x LED (Hijau, Biru, Merah, Putih, Kuning)
- [x] Laptop/PC dengan WiFi
- [x] Power supply 5V 3A minimum
- [x] Kabel jumper & breadboard
- [x] 3x Tempat sampah (Organik, Anorganik, B3)
- [x] 1x Penadah sementara dengan servo
- [x] 1x Platform putar dengan servo

### Software yang Dibutuhkan:
- [x] Arduino IDE 2.x
- [x] Python 3.8+
- [x] Google Colab (untuk training model)
- [x] Blynk account (gratis)

---

## ⏱️ Timeline Setup

| Step | Durasi | Keterangan |
|------|--------|------------|
| 1. Hardware Wiring | 20 menit | Rakit komponen |
| 2. Upload ESP32 Code | 15 menit | Flash firmware |
| 3. Training Model | 30 menit | Google Colab |
| 4. Setup Laptop | 10 menit | Install Python packages |
| 5. Setup Blynk | 10 menit | Dashboard config |
| 6. Kalibrasi | 15 menit | Fine-tune servo & sensor |
| 7. Testing | 30 menit | Full system test |
| **TOTAL** | **~90 menit** | **DEMO READY!** |

---

## 🔧 STEP 1: Hardware Wiring (20 menit)

### 1.1. Lihat Hardware Wiring Guide
Buka file `docs/HARDWARE_WIRING.md` untuk detail lengkap koneksi pin.

### 1.2. Quick Connection Checklist:

**ESP32 Main Controller:**
```
✓ Servo Penadah    → GPIO 12
✓ Servo Platform   → GPIO 13
✓ Ultrasonik Org   → GPIO 26 (TRIG), 27 (ECHO)
✓ Ultrasonik Anorg → GPIO 14 (TRIG), 25 (ECHO)
✓ Ultrasonik B3    → GPIO 32 (TRIG), 33 (ECHO)
✓ LED Organik      → GPIO 18
✓ LED Anorganik    → GPIO 19
✓ LED B3           → GPIO 21
✓ LED System       → GPIO 22
✓ LED Error        → GPIO 23
✓ Power            → 5V & GND
```

**ESP32-CAM:**
```
✓ Camera Module    → Pre-installed
✓ Power            → 5V & GND (3A minimum!)
✓ LED Flash        → GPIO 4 (built-in)
```

### 1.3. Mekanik Assembly:
1. Pasang servo penadah di bawah container sementara
2. Pasang servo platform untuk rotasi
3. Letakkan 3 tempat sampah di posisi 0°, 90°, 180°
4. Pasang sensor ultrasonik di atas setiap bin
5. Pasang ESP32-CAM di atas penadah sementara (menghadap ke bawah)

---

## 📱 STEP 2: Upload ESP32 Code (15 menit)

### 2.1. Install Arduino IDE
Download dari: https://www.arduino.cc/en/software

### 2.2. Install ESP32 Board
1. Buka Arduino IDE
2. File → Preferences
3. Additional Board Manager URLs:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
4. Tools → Board → Board Manager
5. Search "ESP32" → Install "esp32 by Espressif Systems"

### 2.3. Install Required Libraries
Tools → Manage Libraries, install:
- **ESP32Servo** (by Kevin Harrington)
- **Blynk** (by Volodymyr Shymanskyy)

### 2.4. Upload ke ESP32 Main Controller

1. **Buka file**: `esp32/esp32_main_controller/esp32_main_controller.ino`

2. **Edit WiFi Credentials** (line 48-49):
   ```cpp
   const char* ssid = "NamaWiFiAnda";        // GANTI!
   const char* password = "PasswordWiFi";     // GANTI!
   ```

3. **Edit Blynk Token** (line 52):
   ```cpp
   char auth[] = "YourBlynkToken";           // GANTI! (dari step 5)
   ```

4. **Select Board**:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module

5. **Select Port**:
   - Tools → Port → (pilih port ESP32 Anda, misal: COM3)

6. **Upload**:
   - Sketch → Upload
   - Tunggu "Hard resetting via RTS pin..."

7. **Test**:
   - Tools → Serial Monitor (115200 baud)
   - Cek output: "✅ SISTEM SIAP!"

### 2.5. Upload ke ESP32-CAM

1. **Buka file**: `esp32/esp32cam_motion_capture/esp32cam_motion_capture.ino`

2. **Edit WiFi & Laptop IP** (line 40-43):
   ```cpp
   const char* ssid = "NamaWiFiAnda";
   const char* password = "PasswordWiFi";
   const char* laptopIP = "192.168.1.100";    // IP laptop Anda
   ```

3. **Select Board**:
   - Tools → Board → ESP32 Arduino → AI Thinker ESP32-CAM

4. **Upload** (PENTING!):
   - Sambungkan GPIO 0 ke GND (untuk mode flash)
   - Klik Upload
   - Setelah "Connecting....", lepas GPIO 0
   - Tunggu selesai
   - Press RESET button

5. **Test**:
   - Serial Monitor (115200 baud)
   - Cek IP address yang didapat
   - Buka browser: `http://<IP-ESP32-CAM>/stream`

---

## 🤖 STEP 3: Training Model YOLOv8 (30 menit)

### 3.1. Buka Google Colab
File `yolov8_pemilah_sampah_otomatis.ipynb` sudah tersedia di repository.

### 3.2. Upload ke Colab
1. Buka https://colab.research.google.com
2. File → Upload notebook
3. Pilih file `.ipynb` dari repository

### 3.3. Setup Dataset
1. Daftar di Roboflow: https://roboflow.com
2. Upload dataset sampah (Organik, Anorganik, B3)
3. Annotate jika belum
4. Export format: YOLOv8
5. Copy API key

### 3.4. Run Training
1. Edit cell 3 (ganti API key)
2. Runtime → Run all
3. Tunggu ~20-30 menit
4. Model akan tersimpan di Google Drive: `best.pt`

### 3.5. Download Model
1. Download `best.pt` dari Google Drive
2. Copy ke folder: `models/best.pt` di laptop

---

## 💻 STEP 4: Setup Laptop Inference (10 menit)

### 4.1. Install Python Dependencies
```bash
cd sistem-pemilah-sampah-cerdas-IOT-YOLOV8
pip install -r requirements.txt
```

### 4.2. Edit Configuration
1. Copy `config/config.py` → `config/config.py` (jika belum ada)
2. Edit settings:
   ```python
   # WiFi
   WIFI_SSID = "NamaWiFiAnda"
   WIFI_PASSWORD = "PasswordWiFi"
   
   # IP Addresses (cek dari Serial Monitor ESP32)
   ESP32CAM_IP = "192.168.1.101"      # IP ESP32-CAM
   ESP32_MAIN_IP = "192.168.1.102"    # IP ESP32 Main
   LAPTOP_IP = "192.168.1.100"        # IP laptop (ipconfig/ifconfig)
   
   # Model
   MODEL_PATH = "models/best.pt"      # Path ke model
   ```

### 4.3. Test Run
```bash
python inference/laptop_inference_dual.py
```

Expected output:
```
🗑️  SISTEM PEMILAH SAMPAH CERDAS - LAPTOP INFERENCE
====================================================================
🤖 Loading YOLOv8 model...
   ✓ Model loaded: models/best.pt
   ✓ Model test OK
✅ SISTEM SIAP!
====================================================================
🌐 Flask server akan berjalan di http://0.0.0.0:5000
```

---

## ☁️ STEP 5: Setup Blynk (10 menit)

### 5.1. Daftar Blynk
1. Buka: https://blynk.cloud
2. Sign up (gratis)

### 5.2. Create Template
1. Developer Zone → My Templates → New Template
2. Name: "Sistem Pemilah Sampah"
3. Hardware: ESP32
4. Connection: WiFi

### 5.3. Setup Datastreams (Virtual Pins)
Tambahkan datastreams berikut:

| Pin | Name | Type | Min | Max |
|-----|------|------|-----|-----|
| V0 | Counter Organik | Integer | 0 | 1000 |
| V1 | Counter Anorganik | Integer | 0 | 1000 |
| V2 | Counter B3 | Integer | 0 | 1000 |
| V3 | Total Processed | Integer | 0 | 5000 |
| V4 | System Status | String | - | - |
| V5 | Bin Status | String | - | - |
| V7 | Reset Button | Integer | 0 | 1 |

### 5.4. Create Web Dashboard
1. Web Dashboard → Edit
2. Tambahkan widgets:
   - **Value Display** untuk V0, V1, V2, V3
   - **Label** untuk V4 (System Status)
   - **Label** untuk V5 (Bin Status)
   - **Button** untuk V7 (Reset)

### 5.5. Get Auth Token
1. Search → Devices
2. New Device → From Template
3. Copy **Auth Token**
4. Paste ke `config/config.py`:
   ```python
   BLYNK_AUTH = "YourAuthTokenHere"
   ```

### 5.6. Setup Notifications
1. Settings → Events
2. Add Event: `bin_full`
3. Enable notifications

Lihat detail di `docs/BLYNK_SETUP.md`

---

## ⚙️ STEP 6: Kalibrasi Sistem (15 menit)

### 6.1. Kalibrasi Servo Penadah
1. Uncomment line 105 di `esp32_main_controller.ino`:
   ```cpp
   calibrationMode();  // UNCOMMENT INI
   ```
2. Upload ulang ke ESP32
3. Buka Serial Monitor
4. Pilih menu "1" (Test Servo Penadah)
5. Catat angle CLOSE dan OPEN yang pas
6. Update di line 55-56:
   ```cpp
   int SERVO_PENADAH_CLOSE = 0;    // Ganti dengan nilai yang pas
   int SERVO_PENADAH_OPEN = 90;    // Ganti dengan nilai yang pas
   ```

### 6.2. Kalibrasi Servo Platform
1. Menu "2" (Test Servo Platform)
2. Catat angle untuk tiap bin:
   - HOME (posisi awal)
   - Organik (bin 1)
   - Anorganik (bin 2)
   - B3 (bin 3)
3. Update line 60-63

### 6.3. Kalibrasi Sensor Ultrasonik
1. Menu "3" (Test Ultrasonic)
2. Ukur jarak saat bin KOSONG (harusnya >10cm)
3. Ukur jarak saat bin FULL (misal: 5cm)
4. Set threshold di line 67-69:
   ```cpp
   int THRESHOLD_ORGANIK_FULL = 5;     // cm
   int THRESHOLD_ANORGANIK_FULL = 5;
   int THRESHOLD_B3_FULL = 5;
   ```

### 6.4. Finalisasi
1. Comment kembali `calibrationMode()`
2. Upload ulang
3. System siap!

Detail lengkap: `docs/CALIBRATION_GUIDE.md`

---

## ✅ STEP 7: Testing & Demo (30 menit)

### 7.1. Test Individual Components

**Test 1: WiFi Connection**
```
✓ ESP32 Main terhubung WiFi
✓ ESP32-CAM terhubung WiFi
✓ Laptop di network yang sama
```

**Test 2: Communication**
```bash
# Test ESP32 Main status
curl http://192.168.1.102/status

# Test ESP32-CAM capture
curl http://192.168.1.101/capture -o test.jpg
```

**Test 3: Servo & Sensor**
```
✓ Servo penadah buka/tutup smooth
✓ Servo platform rotasi ke 3 posisi
✓ Sensor ultrasonik baca jarak akurat
✓ LED indicators menyala sesuai kategori
```

### 7.2. Test Full System Flow

1. **Start Laptop Inference**:
   ```bash
   python inference/laptop_inference_dual.py
   ```

2. **Taruh sampah** di penadah sementara

3. **ESP32-CAM** deteksi motion → capture foto

4. **Laptop** terima foto → run inference

5. **Laptop** kirim hasil (0/1/2) ke ESP32 Main

6. **ESP32 Main**:
   - Cek bin penuh? (sensor ultrasonik)
   - Putar platform ke posisi
   - Buka penadah
   - Sampah jatuh
   - Tutup penadah
   - Platform ke HOME

7. **Cek Blynk Dashboard**:
   - Counter terupdate
   - Status bin terupdate

### 7.3. Test Different Waste Types

- [ ] Test sampah **ORGANIK** (buah, sayur) → Bin 0
- [ ] Test sampah **ANORGANIK** (plastik, kertas) → Bin 1
- [ ] Test sampah **B3** (baterai, lampu) → Bin 2

### 7.4. Test Error Handling

- [ ] Test bin FULL (tutup bin dengan tangan)
- [ ] Test WiFi disconnect (matikan WiFi sebentar)
- [ ] Test Serial fallback

### 7.5. Performance Check

- [ ] FPS > 5 (cek di GUI)
- [ ] Latency < 2 detik (capture → hasil)
- [ ] Akurasi > 70% (sesuai model training)

---

## 🎉 Demo Ready Checklist

Sebelum presentasi UAS, pastikan:

- [x] **Hardware**:
  - [ ] Semua komponen terpasang rapi
  - [ ] Power supply stabil
  - [ ] Mekanik smooth (servo, platform)

- [x] **Software**:
  - [ ] ESP32 Main running (LED System blink)
  - [ ] ESP32-CAM running (bisa stream)
  - [ ] Laptop inference running (Flask server)
  - [ ] Blynk dashboard online

- [x] **Testing**:
  - [ ] 3 kategori sampah terdeteksi benar
  - [ ] Bin full detection works
  - [ ] Dashboard terupdate real-time

- [x] **Presentation**:
  - [ ] Slide/video demo ready
  - [ ] Backup plan (jika WiFi bermasalah → Serial)
  - [ ] Sample waste untuk demo

---

## 🆘 Troubleshooting

Jika ada masalah, lihat:
- `docs/TROUBLESHOOTING.md` - Solusi masalah umum
- Serial Monitor output - Debug info detail
- LED indicators - Visual error codes

---

## 📚 Next Steps

Setelah basic system jalan:
1. Fine-tune model (tambah dataset)
2. Optimize servo speed & angles
3. Add sound notifications
4. Polish GUI display
5. Create demo video

---

## 🎓 Untuk Presentasi UAS

### Poin Penting yang Harus Dijelaskan:

1. **Workflow Sistem** (diagram)
2. **YOLOv8 Inference** (tunjukkan deteksi real-time)
3. **IoT Integration** (Blynk dashboard)
4. **Hardware Integration** (servo, sensor)
5. **Error Handling** (bin full, WiFi fallback)

### Demo Flow:
1. Show Blynk dashboard (counter: 0)
2. Taruh sampah organik → deteksi → bin 0
3. Taruh sampah anorganik → deteksi → bin 1
4. Taruh sampah B3 → deteksi → bin 2
5. Show Blynk dashboard (counter terupdate)
6. Show bin full detection (tutup bin → LED error)

---

**Good luck dengan UAS! 🚀**

Jika ada pertanyaan, cek dokumentasi lengkap di folder `docs/`
