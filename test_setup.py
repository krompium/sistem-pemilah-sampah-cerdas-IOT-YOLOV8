#!/usr/bin/env python3
"""
Test script untuk verifikasi instalasi dependencies
"""

import sys

def test_imports():
    """Test all required imports"""
    
    print("=" * 60)
    print("🧪 TESTING DEPENDENCIES")
    print("=" * 60)
    
    results = []
    
    # Test imports
    tests = [
        ("ultralytics", "YOLOv8"),
        ("cv2", "OpenCV"),
        ("numpy", "NumPy"),
        ("PIL", "Pillow"),
        ("yaml", "PyYAML"),
        ("matplotlib", "Matplotlib"),
        ("serial", "PySerial"),
    ]
    
    for module, name in tests:
        try:
            __import__(module)
            print(f"✅ {name:20} - OK")
            results.append(True)
        except ImportError as e:
            print(f"❌ {name:20} - MISSING")
            print(f"   Error: {e}")
            results.append(False)
    
    print("\n" + "=" * 60)
    
    if all(results):
        print("✅ ALL DEPENDENCIES OK!")
        print("\n🚀 Ready untuk training & inference!")
        return True
    else:
        print("❌ SOME DEPENDENCIES MISSING!")
        print("\n💡 Install dengan:")
        print("   pip install -r requirements.txt")
        return False

def test_model():
    """Test if model file exists"""
    
    print("\n" + "=" * 60)
    print("🤖 CHECKING MODEL FILE")
    print("=" * 60)
    
    from pathlib import Path
    
    model_path = Path("models/best.pt")
    
    if model_path.exists():
        size_mb = model_path.stat().st_size / (1024 * 1024)
        print(f"✅ Model found: {model_path}")
        print(f"   Size: {size_mb:.2f} MB")
        return True
    else:
        print(f"⚠️ Model not found: {model_path}")
        print("\n💡 Langkah berikutnya:")
        print("   1. Training model di Google Colab")
        print("   2. Download best.pt")
        print("   3. Simpan di folder: models/best.pt")
        return False

def test_gpu():
    """Test GPU availability"""
    
    print("\n" + "=" * 60)
    print("🎮 CHECKING GPU")
    print("=" * 60)
    
    try:
        import torch
        
        print(f"PyTorch version: {torch.__version__}")
        
        if torch.cuda.is_available():
            print(f"✅ GPU available: {torch.cuda.get_device_name(0)}")
            print(f"   CUDA version: {torch.version.cuda}")
            print(f"   Memory: {torch.cuda.get_device_properties(0).total_memory / 1e9:.2f} GB")
            return True
        else:
            print("⚠️ GPU not available - will use CPU")
            print("\n💡 CPU inference lebih lambat tapi tetap bisa jalan")
            return False
    except ImportError:
        print("❌ PyTorch not installed")
        return False

def main():
    """Main test function"""
    
    print("\n")
    print("╔" + "=" * 58 + "╗")
    print("║  🗑️  SISTEM PEMILAH SAMPAH CERDAS - SETUP TEST         ║")
    print("╚" + "=" * 58 + "╝")
    print()
    
    # Test dependencies
    deps_ok = test_imports()
    
    # Test model
    model_ok = test_model()
    
    # Test GPU
    gpu_ok = test_gpu()
    
    # Summary
    print("\n" + "=" * 60)
    print("📊 SUMMARY")
    print("=" * 60)
    print(f"Dependencies: {'✅ OK' if deps_ok else '❌ ERROR'}")
    print(f"Model file:   {'✅ OK' if model_ok else '⚠️ NOT FOUND'}")
    print(f"GPU:          {'✅ AVAILABLE' if gpu_ok else '⚠️ CPU ONLY'}")
    
    print("\n" + "=" * 60)
    
    if deps_ok:
        if model_ok:
            print("🎉 READY TO GO!")
            print("\n🚀 Run inference:")
            print("   python inference/laptop_inference.py")
        else:
            print("⏳ ALMOST READY!")
            print("\n📝 Next steps:")
            print("   1. Training di Colab")
            print("   2. Download best.pt")
            print("   3. Run inference")
    else:
        print("🔧 SETUP NEEDED!")
        print("\n📝 Install dependencies:")
        print("   pip install -r requirements.txt")
    
    print("=" * 60)
    print()
    
    return deps_ok

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
