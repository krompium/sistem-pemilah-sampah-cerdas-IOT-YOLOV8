# Models Directory

This directory should contain your trained YOLOv8 model.

## How to Get the Model:

1. **Train using Google Colab**:
   - Open `yolov8_pemilah_sampah_otomatis.ipynb` in Google Colab
   - Follow training instructions (~30 minutes)
   - Model will save to Google Drive

2. **Download Model**:
   - Download `best.pt` from Google Drive
   - Place it in this directory: `models/best.pt`

3. **Expected File**:
   ```
   models/
   └── best.pt  (50-100MB, YOLOv8m trained model)
   ```

## Alternative: Pre-trained Model

If you don't have a trained model yet, you can temporarily use a generic YOLOv8 model for testing:

```bash
# Download YOLOv8m pretrained (not for waste detection, just for testing system)
wget https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8m.pt -O models/best.pt
```

**Note**: This generic model won't detect waste categories correctly. You MUST train your own model for actual waste sorting!

## Model Specifications:

- **Architecture**: YOLOv8m (medium)
- **Classes**: 3
  - 0: Organik (organic waste)
  - 1: Anorganik (inorganic waste)
  - 2: B3 (hazardous waste)
- **Input Size**: 640x640
- **Format**: PyTorch (.pt)

## Troubleshooting:

### Model not found error:
```
❌ Error: Model tidak ditemukan di models/best.pt
```

**Solution**: Make sure `best.pt` exists in this directory.

### Model file too small:
If `best.pt` is less than 10MB, it's probably corrupted or not downloaded completely. Re-download from Colab.

### Model inference error:
Make sure you're using YOLOv8 format (not YOLOv5 or other versions).

---

For more information, see `docs/QUICK_START_GUIDE.md`
