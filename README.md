# 🗑️ Sistem Pemilah Sampah Cerdas IoT dengan YOLOv8

Sistem pemilah sampah otomatis berbasis AI untuk membantu pengelolaan sampah rumah tangga menjadi lebih efisien.

## 📋 Deskripsi

Project ini menggunakan YOLOv8m untuk mendeteksi dan mengklasifikasikan sampah menjadi 3 kategori:
- **Organik**: Sisa makanan, daun, ranting
- **Anorganik**: Plastik, kertas, logam, kaca
- **B3**: Bahan Berbahaya & Beracun (baterai, lampu, cat)

## 🔧 Hardware

- ESP32 DevKit V1
- ESP32-CAM AI Thinker
- 3x Servo Motor SG90
- Sensor Ultrasonik HC-SR04
- 3x LED Indicator
- Power Supply 5V 3A

## 💻 Software

- Python 3.8+
- YOLOv8m (Ultralytics)
- Google Colab Pro (untuk training)
- Arduino IDE
- Blynk IoT Platform

## 🚀 Quick Start

### 1. Training Model

```bash
# Buka Google Colab
# Upload: notebooks/Training_YOLOv8m_Colab_Pro.ipynb
# Run all cells
# Download best.pt
```

### 2. Setup Hardware

```bash
# Upload ESP32 code
arduino/esp32_main/esp32_main.ino

# Upload ESP32-CAM code
arduino/esp32cam/esp32cam_capture.ino
```

### 3. Run Inference

```bash
pip install -r requirements.txt
python inference/laptop_inference.py
```

## 📚 Dokumentasi

- [Quick Start Guide](docs/QUICK_START.md)
- [Training Guide](docs/TRAINING_GUIDE.md)
- [Hardware Setup](docs/HARDWARE_SETUP.md)
- [Roboflow Guide](docs/ROBOFLOW_GUIDE.md)

## 👤 Author

**krompium**

## 📄 License

MIT License
