"""
SISTEM PEMILAH SAMPAH CERDAS - LAPTOP INFERENCE
================================================

Script untuk menjalankan inference YOLOv8 di laptop dan
mengirim hasil klasifikasi ke ESP32 via Serial.

Requirements:
- Python 3.8+
- Webcam atau ESP32-CAM
- Model: models/best.pt
- ESP32 terhubung via USB

Usage:
    python laptop_inference.py
"""

import cv2
import serial
import serial.tools.list_ports
from ultralytics import YOLO
import time
import numpy as np

# ========== CONFIGURATION ==========
MODEL_PATH = "models/best.pt"
CONF_THRESHOLD = 0.5
SERIAL_BAUD = 115200

# Class names (sesuaikan dengan dataset Anda)
CLASS_NAMES = {
    0: "Organik",
    1: "Anorganik",
    2: "B3"
}

CLASS_COLORS = {
    0: (0, 255, 0),    # Green for Organik
    1: (0, 255, 255),  # Yellow for Anorganik
    2: (0, 0, 255)     # Red for B3
}

# ========== FUNCTIONS ==========

def find_esp32_port():
    """Auto-detect ESP32 serial port."""
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "USB" in port.description or "Serial" in port.description:
            return port.device
    return None

def send_to_esp32(ser, class_id):
    """Send classification result to ESP32."""
    if ser:
        command = f"CLASS:{class_id}\n"
        ser.write(command.encode())
        print(f"📤 Sent to ESP32: {command.strip()}")

def main():
    print("=" * 60)
    print("🗑️ SISTEM PEMILAH SAMPAH CERDAS - INFERENCE")
    print("=" * 60)
    
    # Load model
    print(f"\n📦 Loading model: {MODEL_PATH}")
    try:
        model = YOLO(MODEL_PATH)
        print("✅ Model loaded successfully!")
    except Exception as e:
        print(f"❌ Error loading model: {e}")
        print("\n💡 Solusi:")
        print("   1. Pastikan file best.pt ada di folder models/")
        print("   2. Training model di Google Colab terlebih dahulu")
        return
    
    # Setup serial connection to ESP32
    print("\n🔌 Mencari ESP32...")
    port = find_esp32_port()
    
    if port:
        try:
            ser = serial.Serial(port, SERIAL_BAUD, timeout=1)
            time.sleep(2)  # Wait for connection
            print(f"✅ ESP32 terhubung di port: {port}")
        except Exception as e:
            print(f"⚠️ Tidak dapat terhubung ke ESP32: {e}")
            print("   Lanjut tanpa ESP32 (preview only)")
            ser = None
    else:
        print("⚠️ ESP32 tidak terdeteksi")
        print("   Lanjut tanpa ESP32 (preview only)")
        ser = None
    
    # Open webcam
    print("\n📹 Membuka webcam...")
    cap = cv2.VideoCapture(0)
    
    if not cap.isOpened():
        print("❌ Error: Tidak dapat membuka webcam")
        return
    
    print("✅ Webcam ready!")
    print("\n" + "=" * 60)
    print("🚀 SISTEM SIAP!")
    print("=" * 60)
    print("\nInstruksi:")
    print("   - Tunjukkan sampah ke kamera")
    print("   - Tekan SPACE untuk klasifikasi")
    print("   - Tekan 'q' untuk keluar")
    print("=" * 60)
    
    last_detection = None
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        
        # Make a copy for display
        display_frame = frame.copy()
        
        # Run inference
        results = model(frame, conf=CONF_THRESHOLD, verbose=False)
        
        # Process results
        if len(results[0].boxes) > 0:
            for box in results[0].boxes:
                # Get box coordinates
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                conf = float(box.conf[0])
                cls = int(box.cls[0])
                
                # Get class name and color
                class_name = CLASS_NAMES.get(cls, f"Class {cls}")
                color = CLASS_COLORS.get(cls, (255, 255, 255))
                
                # Draw box
                cv2.rectangle(display_frame, (x1, y1), (x2, y2), color, 2)
                
                # Draw label
                label = f"{class_name}: {conf:.2f}"
                label_size, _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.6, 2)
                cv2.rectangle(display_frame, (x1, y1 - label_size[1] - 10), 
                            (x1 + label_size[0], y1), color, -1)
                cv2.putText(display_frame, label, (x1, y1 - 5),
                          cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 0), 2)
                
                last_detection = (cls, class_name, conf)
        
        # Display info
        info_text = "Press SPACE to classify | 'q' to quit"
        cv2.putText(display_frame, info_text, (10, 30),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
        
        if last_detection:
            cls, class_name, conf = last_detection
            detection_text = f"Last: {class_name} ({conf:.2%})"
            cv2.putText(display_frame, detection_text, (10, 60),
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, CLASS_COLORS.get(cls), 2)
        
        # Show frame
        cv2.imshow("Pemilah Sampah Cerdas", display_frame)
        
        # Handle keyboard input
        key = cv2.waitKey(1) & 0xFF
        
        if key == ord('q'):
            print("\n👋 Keluar dari program...")
            break
        elif key == ord(' '):  # Space key
            if last_detection:
                cls, class_name, conf = last_detection
                print(f"\n📊 Klasifikasi: {class_name} ({conf:.2%})")
                send_to_esp32(ser, cls)
            else:
                print("\n⚠️ Tidak ada deteksi")
    
    # Cleanup
    cap.release()
    cv2.destroyAllWindows()
    if ser:
        ser.close()
    
    print("\n✅ Program selesai!")
    print("=" * 60)

if __name__ == "__main__":
    main()
