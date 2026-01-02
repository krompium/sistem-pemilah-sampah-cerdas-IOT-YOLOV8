# 🗑️ SISTEM PEMILAH SAMPAH CERDAS - PROJECT GUIDE

## 📋 Overview Lengkap

Project ini adalah sistem pemilah sampah otomatis berbasis AI yang menggunakan:
- **YOLOv8m** untuk deteksi dan klasifikasi sampah
- **ESP32** untuk kontrol hardware
- **ESP32-CAM** untuk capture gambar (optional)
- **Google Colab Pro** untuk training model
- **Python** untuk inference di laptop

---

## 🎯 UNTUK UAS BESOK - CHECKLIST CEPAT

### ⏰ Timeline Rekomendasi (Total: ~4-5 jam)

**FASE 1: Training Model (2 jam)**
1. ✅ Upload notebook ke Google Colab
2. ✅ Setup GPU (Runtime > Change runtime type > GPU)
3. ✅ Ganti API key Roboflow
4. ✅ Run all cells
5. ✅ Download best.pt

**FASE 2: Hardware Setup (1 jam)** - OPTIONAL
1. ✅ Rakit ESP32 + Servo + LED
2. ✅ Upload code ke ESP32
3. ✅ Test servo & LED

**FASE 3: Testing (1 jam)**
1. ✅ Install Python dependencies
2. ✅ Run inference script
3. ✅ Test deteksi dengan webcam
4. ✅ Record video demo

**FASE 4: Presentasi (30 menit)**
1. ✅ Siapkan slides
2. ✅ Demo video
3. ✅ Penjelasan cara kerja

---

## 📁 Struktur Project

```
sistem-pemilah-sampah-cerdas-IOT-YOLOV8/
│
├── 📓 notebooks/
│   └── Training_YOLOv8m_Colab_Pro.ipynb    ← TRAINING NOTEBOOK (PRIORITY #1)
│
├── 🤖 models/
│   └── best.pt                              ← Model hasil training (download dari Colab)
│
├── 🐍 inference/
│   └── laptop_inference.py                  ← Script untuk deteksi di laptop
│
├── 🔧 esp32/
│   └── esp32_main/
│       └── esp32_main.ino                   ← Code untuk ESP32 main controller
│
├── 📷 arduino/
│   └── esp32cam/
│       └── esp32cam_capture.ino             ← Code untuk ESP32-CAM (optional)
│
├── 📚 docs/
│   ├── QUICK_START.md                       ← Panduan cepat
│   ├── TRAINING_GUIDE.md                    ← Detail training
│   ├── HARDWARE_SETUP.md                    ← Wiring diagram
│   └── ROBOFLOW_GUIDE.md                    ← Dataset management
│
├── 📄 README.md                             ← Project overview
├── 📦 requirements.txt                      ← Python dependencies
└── 🚫 .gitignore                            ← Git ignore rules
```

---

## 🚀 CARA PAKAI - STEP BY STEP

### STEP 1: Training Model di Google Colab ⭐⭐⭐ PRIORITY!

**File**: `notebooks/Training_YOLOv8m_Colab_Pro.ipynb`

1. **Buka Google Colab**
   - URL: https://colab.research.google.com

2. **Upload Notebook**
   - File > Upload notebook
   - Pilih: `notebooks/Training_YOLOv8m_Colab_Pro.ipynb`
   - Atau: Open from GitHub > paste repo URL

3. **Setup GPU**
   - Runtime > Change runtime type
   - Hardware accelerator: **GPU**
   - GPU type: T4 (free) atau A100 (Colab Pro)
   - Save

4. **Dapatkan Roboflow API Key**
   - Daftar gratis: https://roboflow.com
   - Login: https://app.roboflow.com
   - Settings > Roboflow API
   - Copy **Private API Key**

5. **Edit Cell 3**
   ```python
   ROBOFLOW_API_KEY = "YOUR_API_KEY_HERE"  # 👈 GANTI INI!
   ```
   Ganti dengan API key Anda

6. **Run All Cells**
   - Runtime > Run all
   - Atau: Ctrl+F9
   - Tunggu ~1-2 jam

7. **Download Model**
   - Cell terakhir akan auto-download `best.pt`
   - Simpan di folder: `models/best.pt`

**✅ SELESAI! Model siap digunakan**

---

### STEP 2: Setup Python Environment

1. **Install Python 3.8+**
   - Download: https://python.org
   - Atau gunakan Anaconda

2. **Clone Repository**
   ```bash
   git clone https://github.com/krompium/sistem-pemilah-sampah-cerdas-IOT-YOLOV8.git
   cd sistem-pemilah-sampah-cerdas-IOT-YOLOV8
   ```

3. **Install Dependencies**
   ```bash
   pip install -r requirements.txt
   ```

4. **Letakkan Model**
   - Copy `best.pt` dari download Colab
   - Paste ke folder: `models/best.pt`

---

### STEP 3: Test Inference (Tanpa Hardware)

**File**: `inference/laptop_inference.py`

1. **Run Script**
   ```bash
   python inference/laptop_inference.py
   ```

2. **Test Deteksi**
   - Webcam akan membuka
   - Tunjukkan sampah ke kamera
   - Press SPACE untuk klasifikasi
   - Press 'q' untuk keluar

**Expected Output:**
```
🗑️ SISTEM PEMILAH SAMPAH CERDAS - INFERENCE
📦 Loading model: models/best.pt
✅ Model loaded successfully!
📹 Membuka webcam...
✅ Webcam ready!
🚀 SISTEM SIAP!
```

**Jika Error: "Model not found"**
- Pastikan `models/best.pt` ada
- Training dulu di Colab

**Jika Error: "Webcam not found"**
- Check webcam permissions
- Atau gunakan video file

---

### STEP 4: Setup Hardware (OPTIONAL)

**Jika Waktu Terbatas: SKIP langkah ini!**
Fokus ke training + inference di laptop dulu.

**File**: `esp32/esp32_main/esp32_main.ino`

1. **Rakit Hardware**
   - Lihat: `docs/HARDWARE_SETUP.md`
   - Minimal: 1x ESP32 + 1x Servo + 1x LED

2. **Upload Code**
   - Buka Arduino IDE
   - File > Open > `esp32_main.ino`
   - Tools > Board > ESP32 Dev Module
   - Tools > Port > [pilih port]
   - Upload

3. **Test Hardware**
   - Open Serial Monitor (115200 baud)
   - LED akan blink 3x
   - Kirim command: `0` (Organik), `1` (Anorganik), `2` (B3)
   - Servo akan bergerak

4. **Connect dengan Inference**
   - ESP32 tetap terhubung via USB
   - Run: `python inference/laptop_inference.py`
   - Script akan auto-detect ESP32

**Expected Flow:**
```
Sampah terdeteksi → Klasifikasi → Kirim ke ESP32 → Servo bergerak
```

---

## 🎓 UNTUK PRESENTASI UAS

### Demo yang Harus Ditampilkan

1. **Training Process** (Screenshot dari Colab)
   - Dataset statistics
   - Training progress
   - Metrics (mAP, precision, recall)
   - Confusion matrix

2. **Inference Demo** (Video/Live)
   - Deteksi sampah organik ✅
   - Deteksi sampah anorganik ✅
   - Confidence score ditampilkan
   - (Optional) Servo bergerak

3. **Code Explanation**
   - Arsitektur YOLOv8m
   - Training configuration
   - Inference pipeline
   - ESP32 integration

### Poin-Poin Penting untuk Dijelaskan

**1. Mengapa YOLOv8m?**
- Balance antara speed & accuracy
- Cocok untuk real-time detection
- File size reasonable (~50MB)

**2. Dataset**
- Source: Roboflow
- Classes: Organik, Anorganik (+ B3 optional)
- Total images: 500+
- Augmentation: Flip, rotation, brightness

**3. Training**
- Platform: Google Colab Pro (GPU)
- Epochs: 150
- Batch size: 32
- Optimizer: AdamW
- Duration: ~1-2 jam

**4. Results**
- mAP50: [lihat hasil training]
- Precision: [lihat hasil training]
- Recall: [lihat hasil training]

**5. Hardware Integration**
- ESP32 untuk kontrol
- Servo untuk pemilahan
- LED untuk indikator
- Serial communication

---

## 🔧 TROUBLESHOOTING

### Problem: Colab GPU tidak tersedia
**Solution:**
- Runtime > Factory reset runtime
- Runtime > Change runtime type > GPU
- Atau tunggu beberapa menit

### Problem: Dataset download error
**Solution:**
- Check API key
- Download manual dari Roboflow
- Upload ZIP ke Colab

### Problem: Out of Memory saat training
**Solution:**
- Kurangi batch size: `'batch': 16`
- Atau gunakan model lebih kecil: `yolov8s.pt`

### Problem: Model accuracy rendah
**Solution:**
- Tambah epochs: `'epochs': 200`
- Check dataset balance
- Improve data quality

### Problem: Webcam tidak terdeteksi
**Solution:**
- Check permissions
- Restart aplikasi
- Gunakan external webcam

### Problem: ESP32 tidak terhubung
**Solution:**
- Install driver CH340/CP2102
- Check kabel USB
- Pilih port yang benar

---

## 💡 TIPS & TRICKS

### Jika Waktu Sangat Terbatas (< 3 jam)

**FOKUS UTAMA:**
1. ✅ Training di Colab (2 jam) - WAJIB!
2. ✅ Inference di laptop (30 menit) - WAJIB!
3. ✅ Record demo (15 menit) - WAJIB!
4. ❌ Hardware - SKIP dulu!

**Minimal Requirements:**
- Model trained ✅
- Bisa deteksi via webcam ✅
- Punya video demo ✅
- **= NILAI BAGUS!** 🎉

### Simplified Version

**Gunakan:**
- Webcam laptop (bukan ESP32-CAM)
- Inference di laptop only
- Skip Blynk integration
- Skip multiple servos

**Demo:**
- Tunjukkan deteksi organik/anorganik
- Explain training process
- Show metrics & results

### Advanced Features (Jika Ada Waktu)

- Multiple servo untuk 3 bins
- ESP32-CAM integration
- Blynk dashboard
- LCD display
- Statistics tracking
- Remote monitoring

---

## 📚 RESOURCES

### Documentation
- [Quick Start](docs/QUICK_START.md) - Panduan cepat
- [Training Guide](docs/TRAINING_GUIDE.md) - Detail training
- [Hardware Setup](docs/HARDWARE_SETUP.md) - Wiring & assembly
- [Roboflow Guide](docs/ROBOFLOW_GUIDE.md) - Dataset management

### External Links
- [YOLOv8 Docs](https://docs.ultralytics.com)
- [Roboflow](https://roboflow.com)
- [Google Colab](https://colab.research.google.com)
- [ESP32 Docs](https://docs.espressif.com)

### Support
- GitHub Issues: [Open Issue](https://github.com/krompium/sistem-pemilah-sampah-cerdas-IOT-YOLOV8/issues)

---

## ✅ CHECKLIST FINAL

Sebelum presentasi, pastikan:

**Training:**
- [ ] Model trained successfully
- [ ] best.pt downloaded
- [ ] Metrics recorded (mAP, precision, recall)
- [ ] Screenshot training results
- [ ] Screenshot confusion matrix

**Inference:**
- [ ] Dependencies installed
- [ ] Webcam working
- [ ] Detection working
- [ ] Confidence scores displayed

**Demo:**
- [ ] Video demo recorded (min 2 menit)
- [ ] Deteksi organik works ✅
- [ ] Deteksi anorganik works ✅
- [ ] (Optional) Hardware demo

**Presentation:**
- [ ] Slides prepared
- [ ] Code explanation ready
- [ ] Q&A preparation

---

## 🎉 GOOD LUCK!

**Remember:**
- Focus pada core functionality dulu
- Training + Inference = Paling penting
- Hardware = Nice to have
- Demo video = Must have
- Confidence dalam presentasi!

**Yang penting:**
- Model trained ✅
- Bisa deteksi sampah ✅
- Paham cara kerjanya ✅
- **= UAS AMAN!** 🚀

---

## 📞 Contact

**Author**: krompium  
**Repository**: https://github.com/krompium/sistem-pemilah-sampah-cerdas-IOT-YOLOV8

---

**Selamat mengerjakan UAS! Semoga sukses! 🎓**
