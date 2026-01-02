# 🗑️ Sistem Pemilah Sampah Cerdas IoT dengan YOLOv8

[![Python](https://img.shields.io/badge/Python-3.8+-blue.svg)](https://python.org)
[![YOLOv8](https://img.shields.io/badge/YOLOv8-v8.0+-green.svg)](https://github.com/ultralytics/ultralytics)
[![ESP32](https://img.shields.io/badge/ESP32-Arduino-red.svg)](https://www.espressif.com/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Sistem pemilah sampah otomatis berbasis AI untuk membantu pengelolaan sampah rumah tangga menjadi lebih efisien menggunakan YOLOv8m dan ESP32.

---

## 🎯 Untuk UAS Besok - Quick Start! ⚡

**Waktu terbatas? Fokus di sini:**

1. **Training Model (2 jam)** ⭐ PRIORITY #1
   - Upload `notebooks/Training_YOLOv8m_Colab_Pro.ipynb` ke Google Colab
   - Setup GPU, ganti API key, Run all cells
   - Download `best.pt`

2. **Test Inference (30 menit)**
   ```bash
   pip install -r requirements.txt
   python test_setup.py  # Cek instalasi
   python inference/laptop_inference.py  # Run deteksi
   ```

3. **Record Demo (15 menit)**
   - Tunjukkan deteksi organik & anorganik
   - Screenshot training results
   - **Done! UAS ready!** ✅

📖 **Panduan Lengkap**: Lihat [PROJECT_GUIDE.md](PROJECT_GUIDE.md)

---

## 📋 Deskripsi

Project ini menggunakan YOLOv8m untuk mendeteksi dan mengklasifikasikan sampah menjadi kategori:
- **Organik**: Sisa makanan, daun, ranting
- **Anorganik**: Plastik, kertas, logam, kaca
- **B3** (Optional): Bahan Berbahaya & Beracun

### Fitur Utama
- ✅ AI Detection dengan YOLOv8m (mAP > 85%)
- ✅ Real-time inference via webcam
- ✅ ESP32 integration untuk hardware control
- ✅ Automated sorting dengan servo motors
- ✅ LED indicators untuk visual feedback
- ✅ Complete training pipeline di Google Colab

---

## 🔧 Hardware Requirements

| Komponen | Qty | Keterangan |
|----------|-----|------------|
| ESP32 DevKit V1 | 1 | Main controller |
| ESP32-CAM | 1 | Camera module (optional) |
| Servo SG90 | 3 | Untuk 3 bins |
| HC-SR04 | 1 | Ultrasonic sensor |
| LED 5mm | 3 | Indicators |
| Resistor 220Ω | 3 | Untuk LED |
| Breadboard | 1 | Prototyping |
| Power Supply 5V 3A | 1 | Power |

**Total cost**: ~Rp 200.000

---

## 💻 Software Requirements

- **Python 3.8+**
- **Google Colab** (untuk training)
- **Arduino IDE 2.0+** (untuk ESP32)
- **Dependencies**: Lihat [requirements.txt](requirements.txt)

---

## 🚀 Quick Start

### 1. Clone Repository

```bash
git clone https://github.com/krompium/sistem-pemilah-sampah-cerdas-IOT-YOLOV8.git
cd sistem-pemilah-sampah-cerdas-IOT-YOLOV8
```

### 2. Training Model

```bash
# 1. Buka Google Colab: https://colab.research.google.com
# 2. Upload: notebooks/Training_YOLOv8m_Colab_Pro.ipynb
# 3. Runtime > Change runtime type > GPU
# 4. Edit Cell 3: Ganti API key Roboflow
# 5. Runtime > Run all
# 6. Download best.pt ke folder models/
```

### 3. Setup Python Environment

```bash
# Install dependencies
pip install -r requirements.txt

# Test instalasi
python test_setup.py
```

### 4. Run Inference

```bash
# Inference dengan webcam
python inference/laptop_inference.py

# Controls:
# - SPACE: Klasifikasi
# - Q: Quit
```

### 5. Setup Hardware (Optional)

```bash
# 1. Rakit hardware sesuai docs/HARDWARE_SETUP.md
# 2. Buka Arduino IDE
# 3. Upload: esp32/esp32_main/esp32_main.ino
# 4. Connect ESP32 via USB
# 5. Run inference script (auto-detect ESP32)
```

---

## 📚 Dokumentasi Lengkap

- 📖 **[PROJECT_GUIDE.md](PROJECT_GUIDE.md)** - **Panduan lengkap untuk UAS!**
- 🚀 [Quick Start Guide](docs/QUICK_START.md) - Setup cepat
- 🎓 [Training Guide](docs/TRAINING_GUIDE.md) - Detail training YOLOv8
- 🔧 [Hardware Setup](docs/HARDWARE_SETUP.md) - Wiring & assembly
- 📊 [Roboflow Guide](docs/ROBOFLOW_GUIDE.md) - Dataset management

---

## 📁 Project Structure

```
├── 📓 notebooks/          # Training notebook
├── 🤖 models/             # Trained models
├── 🐍 inference/          # Inference scripts
├── 🔧 esp32/              # ESP32 code
├── 📷 arduino/            # ESP32-CAM code
├── 📚 docs/               # Documentation
├── 📄 README.md           # This file
├── 📖 PROJECT_GUIDE.md    # Comprehensive guide
└── 📦 requirements.txt    # Dependencies
```

---

## 🎯 Hasil yang Diharapkan

### Training Metrics
- **mAP50**: > 0.85 (Excellent!)
- **Precision**: > 0.80
- **Recall**: > 0.80
- **Training time**: 1-2 jam (Colab Pro)

### Inference Performance
- **FPS**: 15-30 (dengan GPU)
- **Latency**: < 100ms per frame
- **Accuracy**: > 85%

---

## 🎥 Demo

Video demo dan screenshot akan ditambahkan setelah testing.

---

## 🐛 Troubleshooting

**Problem**: GPU tidak tersedia di Colab  
**Solution**: Runtime > Factory reset runtime, lalu pilih GPU lagi

**Problem**: Model not found  
**Solution**: Training dulu di Colab, download best.pt

**Problem**: Webcam tidak terdeteksi  
**Solution**: Check permissions, restart script

Lihat troubleshooting lengkap di [PROJECT_GUIDE.md](PROJECT_GUIDE.md)

---

## 🤝 Contributing

Contributions welcome! Silakan buka issue atau pull request.

---

## 👤 Author

**krompium**
- GitHub: [@krompium](https://github.com/krompium)

---

## 📄 License

MIT License - feel free to use for your projects!

---

## 🙏 Acknowledgments

- [Ultralytics YOLOv8](https://github.com/ultralytics/ultralytics)
- [Roboflow](https://roboflow.com)
- [Google Colab](https://colab.research.google.com)
- [ESP32 Community](https://www.espressif.com/)

---

## 🎓 Good Luck untuk UAS!

**Tips terakhir:**
- Focus pada training + inference ✅
- Hardware = nice to have, bukan wajib
- Demo video sangat membantu
- Pahami cara kerja sistem

**Selamat mengerjakan! 🚀**
