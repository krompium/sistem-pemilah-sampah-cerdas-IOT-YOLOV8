# 🗑️ Sistem Pemilah Sampah Cerdas - IoT & YOLOv8

> **Smart Waste Sorting System** menggunakan AI (YOLOv8), ESP32, dan IoT untuk memilah sampah otomatis ke 3 kategori: Organik, Anorganik, dan B3.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![ESP32](https://img.shields.io/badge/ESP32-DevKit%20v1-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![YOLOv8](https://img.shields.io/badge/YOLOv8-Ultralytics-00FFFF.svg)](https://github.com/ultralytics/ultralytics)
[![Blynk](https://img.shields.io/badge/IoT-Blynk-23C48E.svg)](https://blynk.io/)

---

## 🎯 Overview

Sistem pemilah sampah otomatis berbasis AI dengan komponen:

- **🤖 AI Detection**: YOLOv8 untuk klasifikasi sampah
- **📷 ESP32-CAM**: Motion detection & image capture
- **🎛️ ESP32 Main**: Kontrol servo & sensor
- **💻 Laptop Inference**: YOLOv8 processing
- **☁️ Blynk IoT**: Real-time monitoring dashboard
- **🔧 Dual Communication**: WiFi (primary) + Serial (backup)

### Workflow Singkat:

```
Sampah → Motion Detection → Capture Foto → YOLOv8 Inference 
  → Klasifikasi (0/1/2) → Cek Bin Full → Rotasi Platform 
  → Buka Penadah → Sampah Jatuh → Update Dashboard
```

---

## 📦 Hardware Requirements

| Component | Qty | Function |
|-----------|-----|----------|
| ESP32 DevKit V1 | 1 | Main controller |
| ESP32-CAM AI Thinker | 1 | Camera & motion detection |
| Servo Motor (SG90/MG996R) | 2 | Penadah & platform rotation |
| HC-SR04 Ultrasonic | 3 | Bin full detection |
| LED 5mm | 5 | Status indicators |
| Power Supply 5V 3A | 1 | Power |
| Breadboard & Jumper Wires | - | Connections |

**Total Budget**: ~Rp 500.000 - Rp 700.000

---

## 💻 Software Requirements

- **Arduino IDE 2.x** - Upload ESP32 code
- **Python 3.8+** - Laptop inference
- **Google Colab** - Model training (gratis)
- **Blynk Account** - IoT dashboard (gratis)

**Python Libraries**: (Install via `pip install -r requirements.txt`)
- ultralytics (YOLOv8)
- opencv-python
- flask
- pyserial
- blynk-library-python

---

## 🚀 Quick Start (5 Steps)

### 1️⃣ Clone Repository
```bash
git clone https://github.com/krompium/sistem-pemilah-sampah-cerdas-IOT-YOLOV8.git
cd sistem-pemilah-sampah-cerdas-IOT-YOLOV8
```

### 2️⃣ Install Dependencies
```bash
pip install -r requirements.txt
```

### 3️⃣ Upload ESP32 Code
- Edit WiFi credentials di `esp32/esp32_main_controller/esp32_main_controller.ino`
- Upload ke ESP32 Main & ESP32-CAM via Arduino IDE

### 4️⃣ Train Model (Google Colab)
- Open `yolov8_pemilah_sampah_otomatis.ipynb` di Colab
- Training ~30 menit
- Download `best.pt` → folder `models/`

### 5️⃣ Run Laptop Inference
```bash
python inference/laptop_inference_dual.py
```

**✅ System Ready!** Taruh sampah di penadah dan lihat magic happen! 🪄

---

## 📁 Folder Structure

```
sistem-pemilah-sampah-cerdas-IOT-YOLOV8/
│
├── 📂 esp32/
│   ├── esp32_main_controller/          # ESP32 main code
│   │   └── esp32_main_controller.ino
│   └── esp32cam_motion_capture/        # ESP32-CAM code
│       └── esp32cam_motion_capture.ino
│
├── 📂 inference/
│   ├── laptop_inference_dual.py        # Main inference script
│   └── blynk_dashboard.py              # Blynk sync script
│
├── 📂 config/
│   └── config.py                       # Centralized configuration
│
├── 📂 docs/
│   ├── QUICK_START_GUIDE.md            # Setup lengkap 90 menit
│   ├── HARDWARE_WIRING.md              # Wiring diagram
│   ├── CALIBRATION_GUIDE.md            # Kalibrasi servo & sensor
│   ├── BLYNK_SETUP.md                  # Blynk dashboard setup
│   └── TROUBLESHOOTING.md              # Solusi masalah umum
│
├── 📂 models/                          # YOLOv8 trained models
│   └── best.pt                         # (download dari Colab)
│
├── 📓 yolov8_pemilah_sampah_otomatis.ipynb  # Colab training
├── 📄 requirements.txt                 # Python dependencies
├── 📄 .gitignore
└── 📄 README.md
```

---

## 🎓 Documentation

Panduan lengkap tersedia di folder `docs/`:

| Document | Description | Time |
|----------|-------------|------|
| [Quick Start Guide](docs/QUICK_START_GUIDE.md) | Setup sistem dari nol | 90 min |
| [Hardware Wiring](docs/HARDWARE_WIRING.md) | Pin connections & assembly | 20 min |
| [Calibration Guide](docs/CALIBRATION_GUIDE.md) | Kalibrasi servo & sensor | 15 min |
| [Blynk Setup](docs/BLYNK_SETUP.md) | IoT dashboard configuration | 10 min |
| [Troubleshooting](docs/TROUBLESHOOTING.md) | Solusi masalah umum | - |

**📌 Rekomendasi**: Mulai dari `QUICK_START_GUIDE.md` untuk setup step-by-step!

---

## ⚙️ Key Features

### ✅ Implemented:
- [x] YOLOv8 object detection (3 classes)
- [x] ESP32-CAM motion detection
- [x] Dual servo control (penadah + platform)
- [x] 3x ultrasonic bin full detection
- [x] WiFi communication (ESP32 ↔ Laptop)
- [x] Serial fallback communication
- [x] Blynk IoT dashboard
- [x] LED status indicators
- [x] CSV logging
- [x] Real-time GUI display
- [x] Calibration mode
- [x] Error handling & retry

### 🔧 Customizable:
- Servo angles (buka/tutup, rotasi)
- Sensor thresholds (bin full detection)
- WiFi credentials
- Model confidence threshold
- Communication ports
- LED pins
- All parameters di `config/config.py`

---

## 🎥 Demo

### Expected Output:

1. **User** taruh sampah di penadah
2. **ESP32-CAM** deteksi motion → capture foto
3. **Laptop** terima foto → YOLOv8 inference
4. **Hasil**: 0=Organik, 1=Anorganik, 2=B3
5. **ESP32 Main**:
   - Cek bin penuh (ultrasonic)
   - Putar platform ke bin target
   - Buka penadah → sampah jatuh
   - Tutup penadah
   - Platform ke HOME
6. **Blynk** dashboard update counter
7. **LED** nyala sesuai kategori

**⏱️ Total time**: ~3-5 detik per sampah

---

## 🧪 Testing

Basic system test:
```bash
# Test ESP32 status
curl http://192.168.1.102/status

# Test ESP32-CAM capture
curl http://192.168.1.101/capture -o test.jpg

# Manual classification via Serial
# Open Serial Monitor, ketik: 0 (Organik), 1 (Anorganik), 2 (B3)
```

---

## 🤝 Contributing

Contributions welcome! Silakan:
1. Fork repository
2. Create feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open Pull Request

---

## 📝 License

Distributed under the MIT License. See `LICENSE` for more information.

---

## 👨‍💻 Author

**@krompium**

Project untuk: **Tugas UAS - Sistem Pemilah Sampah Cerdas**

---

## 🙏 Acknowledgments

- [Ultralytics YOLOv8](https://github.com/ultralytics/ultralytics) - AI model
- [Espressif ESP32](https://www.espressif.com/) - Hardware platform
- [Blynk](https://blynk.io/) - IoT dashboard
- [Arduino](https://www.arduino.cc/) - Development framework

---

## 📞 Support

Jika ada pertanyaan atau issue:

1. Check [Troubleshooting Guide](docs/TROUBLESHOOTING.md)
2. Open GitHub Issue
3. Check dokumentasi lengkap di folder `docs/`

---

## 🎯 Roadmap

Future improvements:
- [ ] Mobile app (React Native / Flutter)
- [ ] Multi-language detection
- [ ] Cloud model deployment
- [ ] Automatic dataset collection
- [ ] Voice notifications
- [ ] Energy monitoring
- [ ] Waste analytics dashboard

---

**⭐ Star repository ini jika bermanfaat!**

**🔥 Good luck dengan UAS! 🚀**
