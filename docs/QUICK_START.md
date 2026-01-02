# 🚀 Quick Start Guide

Panduan cepat untuk memulai proyek Sistem Pemilah Sampah Cerdas.

## 📋 Persiapan

### 1. Requirements

**Software:**
- Google Colab Pro (untuk training)
- Python 3.8+ (untuk inference)
- Arduino IDE 2.0+
- Git

**Hardware:**
- ESP32 DevKit V1
- ESP32-CAM AI Thinker
- 3x Servo SG90
- Sensor Ultrasonik HC-SR04
- 3x LED
- Breadboard & kabel jumper
- Power Supply 5V 3A

### 2. Clone Repository

```bash
git clone https://github.com/krompium/sistem-pemilah-sampah-cerdas-IOT-YOLOV8.git
cd sistem-pemilah-sampah-cerdas-IOT-YOLOV8
```

## 🎯 Langkah-Langkah

### STEP 1: Training Model (Google Colab)

1. **Buka Google Colab**
   - Kunjungi: https://colab.research.google.com

2. **Upload Notebook**
   - File > Upload notebook
   - Pilih: `notebooks/Training_YOLOv8m_Colab_Pro.ipynb`

3. **Setup GPU**
   - Runtime > Change runtime type
   - Hardware accelerator: **GPU**
   - GPU type: **T4** atau **A100**

4. **Dapatkan Roboflow API Key**
   - Daftar gratis di: https://roboflow.com
   - Login ke: https://app.roboflow.com
   - Settings > Roboflow API
   - Copy **Private API Key**

5. **Edit Notebook**
   - Cell 3: Ganti `YOUR_API_KEY_HERE` dengan API key Anda

6. **Run Training**
   - Runtime > Run all
   - Tunggu 1-2 jam (tergantung GPU)

7. **Download Model**
   - Cell terakhir akan auto-download `best.pt`
   - Simpan di folder `models/`

### STEP 2: Setup Hardware

1. **Wiring Diagram**
   - Lihat: [docs/HARDWARE_SETUP.md](HARDWARE_SETUP.md)

2. **Upload Code ke ESP32**
   ```bash
   # Buka Arduino IDE
   # File > Open > esp32/esp32_main/esp32_main.ino
   # Tools > Board > ESP32 Dev Module
   # Tools > Port > [pilih port ESP32]
   # Upload
   ```

### STEP 3: Run Inference

1. **Install Dependencies**
   ```bash
   pip install -r requirements.txt
   ```

2. **Test Model**
   ```bash
   python inference/laptop_inference.py
   ```

3. **Connect ESP32**
   - Hubungkan ESP32 ke laptop via USB
   - Script akan otomatis detect

4. **Test Deteksi**
   - Letakkan sampah di depan kamera
   - Sistem akan otomatis klasifikasi & pilah

## ✅ Verification

Pastikan semua berjalan:

- [ ] Model trained (best.pt tersimpan)
- [ ] ESP32 terhubung & upload code sukses
- [ ] Inference script berjalan
- [ ] Servo bergerak saat klasifikasi
- [ ] LED menyala sesuai kategori

## 🆘 Troubleshooting

### GPU Tidak Terdeteksi di Colab
- Runtime > Disconnect and delete runtime
- Runtime > Change runtime type > GPU
- Run ulang

### Error Download Dataset
- Cek API key sudah benar
- Download manual dari Roboflow
- Upload ZIP ke Colab

### ESP32 Tidak Terdeteksi
- Install driver CH340/CP2102
- Restart Arduino IDE
- Cek kabel USB

### Inference Lambat
- Gunakan GPU jika tersedia
- Kurangi resolusi kamera
- Gunakan YOLOv8n (lebih cepat)

## 📚 Dokumentasi Lengkap

- [Training Guide](TRAINING_GUIDE.md) - Detail training process
- [Hardware Setup](HARDWARE_SETUP.md) - Wiring & assembly
- [Roboflow Guide](ROBOFLOW_GUIDE.md) - Dataset management

## 💡 Tips

- **Untuk UAS besok:**
  - Fokus ke training + inference di laptop dulu
  - Hardware bisa simplified (1 servo + 1 LED)
  - Blynk optional, bisa di-skip

- **Best Practices:**
  - Simpan model di Google Drive (backup)
  - Test dengan berbagai sampah
  - Record video demo untuk presentasi

## 🎓 Good Luck!

Semoga sukses UAS-nya! 🚀
