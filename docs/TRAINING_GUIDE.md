# 📚 Training Guide - YOLOv8m untuk Pemilah Sampah

Panduan lengkap training model YOLOv8m untuk deteksi dan klasifikasi sampah.

## 📋 Overview

Training dilakukan di **Google Colab Pro** menggunakan:
- **Model**: YOLOv8m (Medium)
- **Dataset**: Roboflow - Sampah Organik & Anorganik
- **GPU**: Tesla T4 / A100
- **Durasi**: 1-2 jam (150 epochs)

## 🎯 Mengapa YOLOv8m?

| Model | Speed | Accuracy | Size | Rekomendasi |
|-------|-------|----------|------|-------------|
| YOLOv8n | ⚡⚡⚡⚡ | ⭐⭐ | 6 MB | Mobile/Edge |
| YOLOv8s | ⚡⚡⚡ | ⭐⭐⭐ | 22 MB | Balanced |
| **YOLOv8m** | **⚡⚡** | **⭐⭐⭐⭐** | **52 MB** | **🏆 Best Choice** |
| YOLOv8l | ⚡ | ⭐⭐⭐⭐⭐ | 87 MB | High Accuracy |
| YOLOv8x | 🐌 | ⭐⭐⭐⭐⭐ | 136 MB | Maximum Accuracy |

**YOLOv8m** dipilih karena:
- Balance antara speed & accuracy
- Cocok untuk ESP32-CAM (inference di laptop)
- Training time reasonable (~1-2 jam)

## 📊 Dataset

### Sumber Dataset

**Roboflow Universe:**
- URL: https://universe.roboflow.com/siscer-project/sampah-organik-dan-anorganik
- Kelas: 2 (Organik, Anorganik)
- Format: YOLOv8
- Images: ~500-1000 (tergantung version)

### Struktur Dataset

```
dataset/
├── train/
│   ├── images/
│   │   ├── img001.jpg
│   │   ├── img002.jpg
│   │   └── ...
│   └── labels/
│       ├── img001.txt
│       ├── img002.txt
│       └── ...
├── valid/
│   ├── images/
│   └── labels/
└── data.yaml
```

### Format Label (YOLO)

Setiap file `.txt` berisi:
```
<class_id> <x_center> <y_center> <width> <height>
```

Contoh:
```
0 0.5 0.5 0.3 0.4
```
- `0` = class ID (0=Organik, 1=Anorganik)
- Koordinat normalized (0-1)

## ⚙️ Training Configuration

### Hyperparameters

```python
CONFIG = {
    # Model
    'model': 'yolov8m.pt',
    
    # Training
    'epochs': 150,        # Jumlah iterasi
    'imgsz': 640,         # Input image size
    'batch': 32,          # Batch size (sesuaikan dengan GPU)
    
    # Optimizer
    'optimizer': 'AdamW', # AdamW > Adam
    'lr0': 0.001,         # Learning rate awal
    'lrf': 0.01,          # Learning rate akhir
    'momentum': 0.937,
    'weight_decay': 0.0005,
    
    # Early Stopping
    'patience': 50,       # Stop jika tidak improve
}
```

### Data Augmentation

```python
'hsv_h': 0.015,       # Hue jitter
'hsv_s': 0.7,         # Saturation
'hsv_v': 0.4,         # Value/brightness
'degrees': 10.0,      # Rotation ±10°
'translate': 0.1,     # Translation 10%
'scale': 0.5,         # Scaling 50%
'flipud': 0.0,        # No vertical flip
'fliplr': 0.5,        # 50% horizontal flip
'mosaic': 1.0,        # Mosaic augmentation
'mixup': 0.15,        # Mixup augmentation
```

## 🚀 Training Process

### 1. Setup Environment

```python
# Install dependencies
!pip install ultralytics roboflow

# Check GPU
import torch
print(torch.cuda.is_available())  # Should be True
```

### 2. Download Dataset

```python
from roboflow import Roboflow

rf = Roboflow(api_key="YOUR_API_KEY")
project = rf.workspace("siscer-project").project("sampah-organik-dan-anorganik")
dataset = project.version(3).download("yolov8")
```

### 3. Start Training

```python
from ultralytics import YOLO

model = YOLO('yolov8m.pt')
results = model.train(
    data='path/to/data.yaml',
    epochs=150,
    imgsz=640,
    batch=32,
    # ... (other params)
)
```

### 4. Monitor Progress

Training akan menampilkan:
```
Epoch  GPU_mem  box_loss  cls_loss  dfl_loss  Instances  Size
  1/150   5.12G    1.234    0.567    1.123        125    640
  2/150   5.12G    1.156    0.523    1.089        125    640
  ...
```

**Metrics:**
- `box_loss`: Localization loss (↓ lebih baik)
- `cls_loss`: Classification loss (↓ lebih baik)
- `mAP50`: Mean Average Precision @ IoU 0.5 (↑ lebih baik)

## 📈 Evaluation Metrics

### Confusion Matrix

Menunjukkan klasifikasi benar vs salah:

```
                Predicted
              Org  Anorg
Actual  Org   85    5
        Anorg  3    87
```

### mAP (mean Average Precision)

- **mAP50**: Precision pada IoU threshold 0.5
- **mAP50-95**: Average dari IoU 0.5 sampai 0.95

**Target Metrics:**
- mAP50 > 0.85 = Excellent! 🎉
- mAP50 > 0.70 = Good ✅
- mAP50 < 0.70 = Need improvement 📈

## 💾 Export Model

### Format Export

1. **PyTorch (.pt)**
   ```python
   # Sudah tersimpan otomatis
   # runs/detect/train/weights/best.pt
   ```

2. **ONNX** (Universal)
   ```python
   model.export(format='onnx')
   ```

3. **TensorFlow Lite** (Mobile/Edge)
   ```python
   model.export(format='tflite')
   ```

## 🔧 Troubleshooting

### Out of Memory Error

**Solusi:**
```python
# Kurangi batch size
'batch': 16,  # dari 32

# Atau gunakan model lebih kecil
'model': 'yolov8s.pt'  # dari yolov8m.pt
```

### Low Accuracy (mAP < 0.70)

**Solusi:**
1. **Tambah data**: Minimal 500+ gambar per kelas
2. **Balance dataset**: Jumlah organik ≈ anorganik
3. **Augmentation**: Tingkatkan variasi data
4. **Epochs**: Tambah jadi 200-300
5. **Model**: Gunakan YOLOv8l/x untuk accuracy lebih tinggi

### Training Stuck

**Solusi:**
```python
# Adjust learning rate
'lr0': 0.0001,  # Lebih kecil
'patience': 100,  # Lebih sabar
```

## 📝 Best Practices

### DO ✅

- Backup model ke Google Drive
- Gunakan early stopping (patience)
- Monitor metrics setiap epoch
- Test pada validation set
- Export ke multiple formats

### DON'T ❌

- Overfit (train loss << val loss)
- Skip validation
- Training tanpa GPU
- Batch size terlalu besar (OOM)
- Lupakan backup model

## 🎓 Tips untuk UAS

**Jika waktu terbatas:**

1. **Kurangi epochs**: 100 sudah cukup
   ```python
   'epochs': 100,  # ~45 menit
   ```

2. **Gunakan pretrained**: YOLOv8m sudah bagus
   ```python
   # Tidak perlu training from scratch
   ```

3. **Focus on inference**: Yang penting model bisa detect
   ```bash
   python inference/laptop_inference.py
   ```

4. **Demo ready**: Record video demo
   - Deteksi organik ✅
   - Deteksi anorganik ✅
   - Servo bergerak ✅

## 📚 References

- [Ultralytics YOLOv8 Docs](https://docs.ultralytics.com)
- [Roboflow Documentation](https://docs.roboflow.com)
- [Google Colab Guide](https://colab.research.google.com)

## 🎉 Selamat Training!

Good luck dengan training-nya! 🚀
