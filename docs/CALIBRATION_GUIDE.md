# ⚙️ Calibration Guide - Sistem Pemilah Sampah Cerdas

Panduan kalibrasi servo dan sensor untuk performa optimal.

---

## 📋 Overview

Kalibrasi diperlukan karena:
- Setiap servo punya karakteristik berbeda
- Posisi bin bisa bervariasi
- Ukuran bin dan penadah berbeda-beda
- Sensor perlu disesuaikan dengan kondisi fisik

**Estimasi waktu**: 15-20 menit

---

## 🔧 Preparation

### 1. Enable Calibration Mode

Edit file `esp32_main_controller.ino`:

**Line 105** - Uncomment calibration mode:
```cpp
void setup() {
  // ... kode lain ...
  
  // Kalibrasi mode (jika diperlukan - uncomment untuk test)
  calibrationMode();  // ← UNCOMMENT INI untuk enable
  
  // System Ready!
  systemReady = true;
  // ...
}
```

### 2. Upload & Open Serial Monitor

1. Upload code ke ESP32
2. Tools → Serial Monitor (115200 baud)
3. Anda akan lihat menu kalibrasi

---

## 🎛️ Part 1: Servo Penadah Calibration

**Tujuan**: Cari angle yang pas untuk BUKA dan TUTUP penadah.

### Steps:

1. **Masuk ke Calibration Menu**
   - Serial Monitor akan tampil menu
   - Ketik `1` + Enter

2. **Test CLOSE Position**
   ```
   Testing CLOSE position...
   ```
   - Servo akan ke posisi CLOSE (default 0°)
   - **Cek**: Apakah penadah BENAR-BENAR TUTUP?
   - Jika tidak, catat angle yang perlu (misal: 10° atau 350°)

3. **Test OPEN Position**
   ```
   Testing OPEN position...
   ```
   - Servo akan ke posisi OPEN (default 90°)
   - **Cek**: Apakah penadah TERBUKA cukup lebar?
   - Jika tidak, catat angle yang perlu (misal: 120°)

4. **Fine Tuning**
   - Edit manual di code untuk test angle tertentu:
   ```cpp
   servoPenadah.write(85);  // Test angle 85°
   delay(2000);
   ```
   - Upload, test, ulangi sampai pas

5. **Update Config**
   - Setelah dapat angle yang pas, update di **line 55-56**:
   ```cpp
   int SERVO_PENADAH_CLOSE = 10;   // ⚙️ Ganti dengan angle CLOSE yang pas
   int SERVO_PENADAH_OPEN = 120;   // ⚙️ Ganti dengan angle OPEN yang pas
   ```

### Expected Results:

- ✅ CLOSE: Penadah tertutup rapat (sampah tidak jatuh)
- ✅ OPEN: Penadah terbuka penuh (sampah jatuh smooth)
- ✅ Movement: Smooth, tidak ada getaran berlebih

### Troubleshooting:

| Problem | Solution |
|---------|----------|
| Servo jitter/bergetar | Cek power supply, pastikan 5V stabil |
| Penadah tidak tutup rapat | Turunkan angle (misal: 0 → -10 atau 355°) |
| Penadah tidak buka penuh | Naikkan angle (misal: 90 → 120°) |
| Servo tidak gerak sama sekali | Cek koneksi signal wire (GPIO 12) |

---

## 🔄 Part 2: Servo Platform Calibration

**Tujuan**: Set posisi rotasi yang tepat untuk tiap bin.

### Steps:

1. **Masuk ke Menu**
   - Ketik `2` + Enter di Serial Monitor

2. **Test HOME Position**
   ```
   Back to HOME...
   ```
   - Platform akan ke posisi HOME (default 0°)
   - **Cek**: Posisi awal yang nyaman
   - Bisa juga di set ke center (90°) jika lebih baik

3. **Test ORGANIK Position**
   ```
   Moving to ORGANIK position...
   ```
   - Platform rotasi ke bin Organik (default 0°)
   - **Cek**: Apakah platform TEPAT di atas bin Organik?
   - **Measure**: Gunakan busur/protractor jika perlu

4. **Test ANORGANIK Position**
   ```
   Moving to ANORGANIK position...
   ```
   - Platform rotasi ke bin Anorganik (default 90°)
   - **Cek**: Platform tepat di atas bin Anorganik?

5. **Test B3 Position**
   ```
   Moving to B3 position...
   ```
   - Platform rotasi ke bin B3 (default 180°)
   - **Cek**: Platform tepat di atas bin B3?

6. **Fine Tuning dengan Test Object**
   - Taruh object kecil di penadah
   - Test rotasi + buka penadah
   - Cek apakah jatuh tepat di bin
   - Adjust angle kalau meleset

7. **Update Config**
   - Update di **line 60-63**:
   ```cpp
   int SERVO_PLATFORM_HOME = 0;          // ⚙️ HOME position
   int SERVO_PLATFORM_ORGANIK = 30;      // ⚙️ Bin Organik (adjust!)
   int SERVO_PLATFORM_ANORGANIK = 120;   // ⚙️ Bin Anorganik (adjust!)
   int SERVO_PLATFORM_B3 = 210;          // ⚙️ Bin B3 (adjust!)
   ```

### Layout Examples:

**Linear Layout** (bins in a row):
```
HOME    BIN1    BIN2    BIN3
 0°     30°     90°     150°
```

**Triangle Layout** (120° spacing):
```
       BIN1 (0°)
         |
         |
BIN2----+----BIN3
(120°)      (240°)
```

**Custom Layout**: Measure dengan protractor!

### Expected Results:

- ✅ Platform rotasi smooth tanpa goyang
- ✅ Tiap posisi TEPAT di atas bin
- ✅ Sampah jatuh ke bin yang benar (tidak meleset)

---

## 📏 Part 3: Ultrasonic Sensor Calibration

**Tujuan**: Set threshold jarak untuk deteksi bin FULL.

### Steps:

1. **Masuk ke Menu**
   - Ketik `3` + Enter di Serial Monitor

2. **Observe Readings**
   ```
   Sample 1: Organik=25.3cm, Anorganik=24.8cm, B3=26.1cm
   Sample 2: Organik=25.1cm, Anorganik=24.9cm, B3=26.0cm
   ...
   ```

3. **Test BIN KOSONG**
   - Kosongkan semua bin
   - Catat jarak yang terbaca (misal: ~25cm)
   - Ini adalah **baseline** (bin kosong)

4. **Test BIN FULL**
   - Isi bin sampai PENUH (atau simulasi dengan tangan)
   - Catat jarak yang terbaca (misal: ~5cm)
   - Ini adalah **threshold full**

5. **Tentukan Threshold**
   - Pilih nilai antara FULL dan KOSONG
   - **Recommended**: 20% dari jarak kosong
   - Contoh: Kosong=25cm → Full threshold=5cm

6. **Update Config**
   - Update di **line 67-69**:
   ```cpp
   int THRESHOLD_ORGANIK_FULL = 5;      // ⚙️ cm (adjust based on test!)
   int THRESHOLD_ANORGANIK_FULL = 5;    // ⚙️ cm
   int THRESHOLD_B3_FULL = 5;           // ⚙️ cm
   ```

### Calculation Example:

```
Bin height: 30cm
Sensor mounted at: 35cm from bottom

Bin EMPTY:
  Distance = 35cm (sensor to bottom)

Bin FULL (sampah 30cm):
  Distance = 5cm (sensor to top of waste)

Threshold = 7cm (buffer untuk detection)
```

### Tips:

- **Jangan terlalu sensitif** (threshold terlalu tinggi)
  - Bisa false alarm
  
- **Jangan terlalu lambat** (threshold terlalu rendah)
  - Bin benar-benar penuh baru detect

- **Test dengan sampah asli**
  - Sampah tidak rata → bisa ada variasi

### Expected Results:

- ✅ Bin kosong: Sensor baca >10cm
- ✅ Bin 80% full: Sensor detect full
- ✅ Konsisten: Reading stabil (tidak fluktuasi banyak)

---

## 🎯 Part 4: System Integration Test

Setelah semua dikalibrasi, test full workflow:

### 1. Disable Calibration Mode

Comment kembali di setup():
```cpp
// calibrationMode();  // ← COMMENT lagi
```

Upload code final.

### 2. Test Full Workflow

**Manual Test via Serial:**
```
1. Open Serial Monitor
2. Ketik: 0 (untuk test Organik)
3. Observe:
   - Platform rotasi ke Organik
   - Penadah buka
   - Delay 2 detik
   - Penadah tutup
   - Platform HOME
   - LED Organik nyala
```

**Test untuk kategori lain:**
- Ketik `1` → Anorganik
- Ketik `2` → B3

### 3. Verify Results:

| Step | Expected | Status |
|------|----------|--------|
| 1. Platform rotasi | Smooth ke posisi yang tepat | ⬜ |
| 2. Delay stabilisasi | 500ms cukup | ⬜ |
| 3. Penadah buka | Terbuka penuh | ⬜ |
| 4. Sampah jatuh | Masuk ke bin yang benar | ⬜ |
| 5. Penadah tutup | Tertutup rapat | ⬜ |
| 6. Platform HOME | Kembali ke posisi awal | ⬜ |
| 7. LED indicator | Nyala sesuai kategori | ⬜ |

### 4. Test Bin Full Detection:

```
1. Tutup bin Organik (pakai tangan/kardus)
2. Sensor ultrasonik baca <5cm
3. Ketik: 0 (test organik)
4. Expected: LED Error nyala, tidak proses
```

---

## 📊 Calibration Data Sheet

Record nilai kalibrasi Anda:

```
========================================
CALIBRATION DATA
Date: __________
========================================

SERVO PENADAH:
  CLOSE angle: _____°
  OPEN angle:  _____°
  Speed: Smooth ☐  Jerky ☐

SERVO PLATFORM:
  HOME:     _____°
  Organik:  _____°
  Anorganik: _____°
  B3:       _____°

ULTRASONIC SENSORS:
  Organik:
    Empty: _____cm
    Full:  _____cm
    Threshold: _____cm
  
  Anorganik:
    Empty: _____cm
    Full:  _____cm
    Threshold: _____cm
  
  B3:
    Empty: _____cm
    Full:  _____cm
    Threshold: _____cm

SYSTEM TEST:
  Organik → Bin 1: ☐ Pass ☐ Fail
  Anorganik → Bin 2: ☐ Pass ☐ Fail
  B3 → Bin 3: ☐ Pass ☐ Fail
  Full detection: ☐ Pass ☐ Fail

NOTES:
_________________________________
_________________________________
_________________________________
========================================
```

---

## 🔄 Re-calibration

**Kapan perlu re-calibrate?**

- ✅ Servo diganti/dipindah
- ✅ Posisi bin berubah
- ✅ Ukuran bin berbeda
- ✅ Servo jadi tidak smooth (aging)
- ✅ Sensor reading tidak akurat

**Quick re-calibration** (tanpa upload ulang):

Bisa adjust di **Blynk** atau **web interface** jika sudah implement dynamic config (advanced feature).

---

## ✅ Final Checklist

Before marking calibration complete:

- [ ] Servo penadah buka/tutup smooth
- [ ] Servo platform rotasi tepat ke 3 bin
- [ ] Sensor detect bin full/kosong akurat
- [ ] Test 5x untuk tiap kategori → 100% masuk bin yang benar
- [ ] LED indicators work
- [ ] No mechanical jam/collision
- [ ] Values recorded di calibration sheet

---

**Next Step**: Lanjut ke `QUICK_START_GUIDE.md` Step 7 (Testing) untuk full system test!
