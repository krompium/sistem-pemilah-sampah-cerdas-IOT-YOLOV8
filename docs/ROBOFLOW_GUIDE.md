# 📊 Roboflow Guide - Dataset Management

Panduan lengkap menggunakan Roboflow untuk dataset management dan labeling.

## 📋 Apa itu Roboflow?

**Roboflow** adalah platform untuk:
- Upload & manage dataset
- Labeling/annotation gambar
- Data augmentation
- Export ke berbagai format (YOLOv8, COCO, etc.)
- Versioning dataset

**Link**: https://roboflow.com

## 🚀 Getting Started

### Step 1: Daftar Akun (GRATIS)

1. Kunjungi: https://app.roboflow.com/signup
2. Pilih salah satu:
   - Sign up with Google
   - Sign up with GitHub
   - Sign up with Email

3. Pilih plan: **Free** (cukup untuk UAS!)
   - 10,000 images
   - Unlimited projects
   - Public datasets only

### Step 2: Dapatkan API Key

1. Login ke: https://app.roboflow.com
2. Klik **Settings** (icon gear, kiri bawah)
3. Pilih **Roboflow API**
4. Copy **Private API Key**
5. Simpan untuk digunakan di Colab

```
API Key format: xxxXXXxxxXXXxxx_XXxxXX
```

⚠️ **JANGAN SHARE** API key ke public!

## 📥 Download Dataset Existing

### Menggunakan Dataset Publik

**Dataset Sampah yang Tersedia:**

1. **Sampah Organik & Anorganik**
   - URL: https://universe.roboflow.com/siscer-project/sampah-organik-dan-anorganik
   - Classes: 2 (organik, anorganik)
   - Images: ~500+
   - Format: YOLOv8

### Download via Web UI

1. **Buka link dataset**
   ```
   https://universe.roboflow.com/siscer-project/sampah-organik-dan-anorganik
   ```

2. **Klik "Download Dataset"**

3. **Pilih Format:**
   - Select: **YOLOv8**
   - Download code snippet akan muncul

4. **Copy Code:**
   ```python
   from roboflow import Roboflow
   
   rf = Roboflow(api_key="YOUR_API_KEY")
   project = rf.workspace("siscer-project").project("sampah-organik-dan-anorganik")
   dataset = project.version(3).download("yolov8")
   ```

5. **Paste di Colab Notebook**
   - Ganti `YOUR_API_KEY` dengan API key Anda
   - Run cell

### Download Manual (ZIP)

Jika API error:

1. Klik **"Download ZIP"**
2. Pilih format **YOLOv8**
3. Download ke laptop
4. Upload ZIP ke Google Colab
5. Extract:
   ```python
   import zipfile
   with zipfile.ZipFile('dataset.zip', 'r') as zip_ref:
       zip_ref.extractall('dataset/')
   ```

## 📤 Upload Dataset Sendiri

### Step 1: Buat Project Baru

1. **Dashboard** > **Create New Project**
2. **Project Name**: `pemilah-sampah-uas`
3. **Project Type**: **Object Detection**
4. **What will your model predict?**: `sampah`
5. **Annotation Group**: `Waste Types`
6. **Create Project**

### Step 2: Upload Images

1. **Upload** tab
2. **Drag & drop** gambar atau **Browse**
3. Supported formats: JPG, PNG
4. Recommended: 
   - Minimal 100 gambar per kelas
   - Resolution: 640x640 atau lebih
   - Diverse angles & lighting

### Step 3: Labeling/Annotation

**Manual Annotation:**

1. Klik gambar untuk annotate
2. Pilih tool: **Bounding Box**
3. Click & drag untuk buat box
4. Label box: `organik` atau `anorganik`
5. Ulangi untuk semua objek di gambar
6. **Save** dan lanjut ke gambar berikutnya

**Tips Annotation:**
- Box harus pas dengan objek (tidak terlalu luas/sempit)
- Include semua objek yang terlihat
- Consistent labeling

**Keyboard Shortcuts:**
- `D`: Next image
- `A`: Previous image
- `S`: Save
- `Ctrl+Z`: Undo

### Step 4: Dataset Split

1. **Generate** tab
2. **Train/Valid/Test Split:**
   - Train: 70%
   - Valid: 20%
   - Test: 10%

3. **Preprocessing:**
   - ✅ Auto-Orient
   - ✅ Resize: 640x640
   - ⬜ Grayscale (tidak perlu)

4. **Augmentation:**
   ```
   ✅ Flip: Horizontal
   ⬜ Flip: Vertical
   ✅ Rotation: ±15°
   ✅ Brightness: ±25%
   ✅ Exposure: ±25%
   ⬜ Blur: (optional)
   ✅ Noise: (optional, max 5%)
   ```

5. **Generate Version**

### Step 5: Export Dataset

1. **Version** tab
2. Pilih version yang baru dibuat
3. **Export** > **YOLOv8**
4. Copy download code
5. Paste di Colab notebook

## 🔄 Data Augmentation

### Mengapa Augmentation?

- Increase dataset size (100 → 500+ images)
- Improve model generalization
- Reduce overfitting
- Handle various conditions

### Recommended Augmentation

**Untuk Sampah:**

| Augmentation | Value | Reason |
|--------------|-------|--------|
| Horizontal Flip | ✅ Yes | Sampah bisa dari arah mana saja |
| Vertical Flip | ❌ No | Gravity always down |
| Rotation | ±15° | Slight angle variation |
| Brightness | ±25% | Indoor/outdoor lighting |
| Exposure | ±15% | Camera settings |
| Saturation | ±25% | Color variation |
| Noise | 2-5% | Camera sensor noise |
| Blur | Slight | Motion blur |

**Apply Augmentation:**

```python
# Di Roboflow UI
Generate > Augmentation:
- Flip: Horizontal (50%)
- Rotate: Between -15° and +15°
- Brightness: Between -25% and +25%
- Exposure: Between -15% and +15%
- Noise: Up to 2% of pixels
```

## 📊 Dataset Health Check

### Check di Roboflow

1. **Health Check** tab
2. Review:
   - ✅ Class balance (~50-50 organik-anorganik)
   - ✅ Enough images (min 100 per class)
   - ✅ Diverse images (various backgrounds)
   - ⚠️ Duplicate images (remove!)
   - ⚠️ Unlabeled images (annotate!)

### Class Distribution

**Good:**
```
Organik:   250 images (50%)
Anorganik: 250 images (50%)
Total:     500 images
```

**Bad:**
```
Organik:   400 images (80%)
Anorganik: 100 images (20%)  ← Imbalanced!
```

**Fix Imbalance:**
- Collect more minority class images
- Or reduce majority class
- Or use weighted loss in training

## 🔧 Advanced Features

### 1. Active Learning

Upload model predictions → Label only uncertain ones

### 2. Version Control

```
v1: Initial dataset (200 images)
v2: Added augmentation (500 images)
v3: Added B3 class (750 images)
```

### 3. API Integration

```python
from roboflow import Roboflow

# Initialize
rf = Roboflow(api_key="YOUR_API_KEY")

# Get project
project = rf.workspace("YOUR_WORKSPACE").project("YOUR_PROJECT")

# Upload images
project.upload(image="path/to/image.jpg")

# Download specific version
dataset = project.version(2).download("yolov8")
```

## 🆘 Troubleshooting

### Error: "API key invalid"

**Solusi:**
1. Copy ulang API key dari Settings
2. Pastikan tidak ada spasi
3. Check internet connection

### Error: "Workspace not found"

**Solusi:**
1. Check workspace name (case-sensitive)
2. Make sure dataset is public
3. Or use your own workspace:
   ```python
   rf.workspace("YOUR_USERNAME").project("...")
   ```

### Download Gagal di Colab

**Solusi:**
1. Download ZIP manual dari web
2. Upload ZIP ke Google Drive
3. Mount Drive di Colab
4. Extract ZIP:
   ```python
   !unzip /content/drive/MyDrive/dataset.zip
   ```

## 💡 Tips untuk UAS

**Quick Setup:**

1. **Gunakan Dataset Existing:**
   ```python
   # Langsung pakai ini, gak perlu labeling!
   project = rf.workspace("siscer-project").project("sampah-organik-dan-anorganik")
   dataset = project.version(3).download("yolov8")
   ```

2. **Atau Upload Minimal:**
   - 50 gambar organik
   - 50 gambar anorganik
   - Label cepat (10 menit)
   - Generate dengan augmentation (→ 200+ images)

3. **Focus on Training:**
   - Dataset setup: 15 menit
   - Training: 1-2 jam
   - Testing: 30 menit
   - Demo ready! ✅

## 📚 Resources

- [Roboflow Docs](https://docs.roboflow.com)
- [YOLOv8 Export Guide](https://docs.roboflow.com/exporting-data/yolov8)
- [Annotation Guide](https://blog.roboflow.com/tips-for-how-to-label-images/)

## 🎓 Best Practices

**DO ✅:**
- Use consistent labeling
- Include diverse images
- Balance classes
- Version your datasets
- Backup important versions

**DON'T ❌:**
- Share API key publicly
- Upload copyrighted images
- Skip validation split
- Over-augment (quality > quantity)
- Delete only version without backup

## 🎉 Ready to Train!

Setelah dataset ready di Roboflow:
1. ✅ Download ke Colab
2. ✅ Start training YOLOv8m
3. ✅ Evaluate hasil
4. ✅ Export model
5. ✅ Good luck UAS! 🚀
