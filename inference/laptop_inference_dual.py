"""
=============================================================================
SISTEM PEMILAH SAMPAH CERDAS - LAPTOP INFERENCE DUAL COMMUNICATION
Script inference YOLOv8 di laptop dengan dual communication (WiFi + Serial)
=============================================================================

FITUR:
✅ Receive image dari ESP32-CAM via WiFi
✅ YOLOv8 inference untuk klasifikasi sampah
✅ Send hasil ke ESP32 via WiFi (primary)
✅ Send hasil via Serial (backup)
✅ GUI display real-time
✅ CSV logging dengan timestamp
✅ Performance metrics (FPS, latency)
✅ Error handling & retry mechanism

WORKFLOW:
1. Flask server terima image dari ESP32-CAM
2. Run YOLOv8 inference
3. Kirim hasil ke ESP32 Main via WiFi
4. Jika WiFi gagal, fallback ke Serial
5. Display hasil di GUI
6. Log ke CSV file

CARA PAKAI:
1. Pastikan model YOLOv8 sudah ada di models/best.pt
2. Update config.py dengan IP dan credentials
3. Run script: python laptop_inference_dual.py
4. Server akan listen di port 5000

Author: @krompium
Untuk: UAS Sistem Pemilah Sampah Cerdas
=============================================================================
"""

import os
import sys
import time
import cv2
import numpy as np
from datetime import datetime
from flask import Flask, request, jsonify
import threading
import requests
import serial
from pathlib import Path
import csv

# Import YOLOv8
try:
    from ultralytics import YOLO
except ImportError:
    print("❌ Error: ultralytics tidak terinstall!")
    print("Install dengan: pip install ultralytics")
    sys.exit(1)

# Import config
try:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'config'))
    from config import *
except ImportError:
    print("⚠️  Warning: config.py tidak ditemukan, menggunakan default config")
    # Default config
    ESP32_MAIN_IP = "192.168.1.102"
    ESP32_MAIN_PORT = 80
    LAPTOP_PORT = 5000
    MODEL_PATH = "models/best.pt"
    CONFIDENCE_THRESHOLD = 0.70
    CLASS_NAMES = {0: "organik", 1: "anorganik", 2: "b3"}
    CLASS_COLORS = {0: (0, 255, 0), 1: (255, 0, 0), 2: (0, 0, 255)}
    SERIAL_PORT_WINDOWS = "COM3"
    SERIAL_PORT_LINUX = "/dev/ttyUSB0"
    SERIAL_BAUD_RATE = 115200
    SERIAL_TIMEOUT = 2
    LOG_DIR = "logs"
    LOG_FILE = "waste_sorting.csv"
    IMAGE_SAVE_DIR = "captured_images"
    SAVE_IMAGES = True
    SHOW_GUI = True
    DEBUG_MODE = True

# ========== GLOBAL VARIABLES ==========
app = Flask(__name__)
model = None
ser = None  # Serial connection
stats = {
    'total_processed': 0,
    'organik': 0,
    'anorganik': 0,
    'b3': 0,
    'fps': 0,
    'avg_latency': 0
}
last_process_time = 0
processing_lock = threading.Lock()

# ========== INISIALISASI ==========
def initialize():
    """
    Inisialisasi sistem
    """
    print("\n" + "=" * 70)
    print("🗑️  SISTEM PEMILAH SAMPAH CERDAS - LAPTOP INFERENCE")
    print("=" * 70)
    print()
    
    # Load model
    global model
    model = load_model()
    
    # Setup directories
    setup_directories()
    
    # Setup CSV logging
    setup_csv_logging()
    
    # Setup serial (optional)
    setup_serial()
    
    print("\n✅ SISTEM SIAP!")
    print("=" * 70)
    print(f"🌐 Flask server akan berjalan di http://0.0.0.0:{LAPTOP_PORT}")
    print("📡 Menunggu image dari ESP32-CAM...")
    print("=" * 70)
    print()

def load_model():
    """
    Load YOLOv8 model
    """
    print("🤖 Loading YOLOv8 model...")
    
    if not os.path.exists(MODEL_PATH):
        print(f"❌ Error: Model tidak ditemukan di {MODEL_PATH}")
        print("Silakan training model terlebih dahulu menggunakan notebook yang tersedia")
        sys.exit(1)
    
    try:
        model = YOLO(MODEL_PATH)
        print(f"   ✓ Model loaded: {MODEL_PATH}")
        
        # Test inference
        dummy_img = np.zeros((640, 640, 3), dtype=np.uint8)
        results = model(dummy_img, verbose=False)
        print("   ✓ Model test OK")
        
        return model
    except Exception as e:
        print(f"❌ Error loading model: {e}")
        sys.exit(1)

def setup_directories():
    """
    Setup directories untuk logging dan images
    """
    print("📁 Setting up directories...")
    
    Path(LOG_DIR).mkdir(exist_ok=True)
    if SAVE_IMAGES:
        Path(IMAGE_SAVE_DIR).mkdir(exist_ok=True)
    
    print("   ✓ Directories ready")

def setup_csv_logging():
    """
    Setup CSV file untuk logging
    """
    print("📝 Setting up CSV logging...")
    
    csv_path = os.path.join(LOG_DIR, LOG_FILE)
    
    # Create CSV dengan header jika belum ada
    if not os.path.exists(csv_path):
        with open(csv_path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([
                'timestamp',
                'predicted_class',
                'class_name',
                'confidence',
                'bin_status',
                'success',
                'communication',
                'latency_ms'
            ])
        print(f"   ✓ CSV created: {csv_path}")
    else:
        print(f"   ✓ CSV exists: {csv_path}")

def setup_serial():
    """
    Setup serial connection (backup communication)
    """
    global ser
    
    print("🔌 Setting up serial connection (backup)...")
    
    try:
        # Detect OS
        if sys.platform.startswith('win'):
            port = SERIAL_PORT_WINDOWS
        else:
            port = SERIAL_PORT_LINUX
        
        ser = serial.Serial(
            port=port,
            baudrate=SERIAL_BAUD_RATE,
            timeout=SERIAL_TIMEOUT
        )
        
        print(f"   ✓ Serial connected: {port}")
        return True
    except Exception as e:
        print(f"   ⚠️  Serial not available: {e}")
        print("   WiFi communication only")
        ser = None
        return False

# ========== FLASK ENDPOINTS ==========
@app.route('/')
def index():
    """
    Homepage
    """
    return """
    <html>
    <head>
        <title>Sistem Pemilah Sampah Cerdas</title>
        <style>
            body { font-family: Arial; margin: 40px; background: #f0f0f0; }
            .container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
            h1 { color: #2c3e50; }
            .stats { display: grid; grid-template-columns: repeat(3, 1fr); gap: 20px; margin: 20px 0; }
            .stat-box { background: #ecf0f1; padding: 20px; border-radius: 5px; text-align: center; }
            .stat-value { font-size: 32px; font-weight: bold; color: #3498db; }
            .stat-label { color: #7f8c8d; margin-top: 5px; }
            .endpoint { background: #f8f9fa; padding: 10px; margin: 10px 0; border-left: 4px solid #3498db; }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>🗑️ Sistem Pemilah Sampah Cerdas</h1>
            <p>Laptop Inference Server - YOLOv8 Detection</p>
            
            <h2>📊 Statistik</h2>
            <div class="stats">
                <div class="stat-box">
                    <div class="stat-value">""" + str(stats['total_processed']) + """</div>
                    <div class="stat-label">Total Processed</div>
                </div>
                <div class="stat-box">
                    <div class="stat-value">""" + str(stats['organik']) + """</div>
                    <div class="stat-label">Organik</div>
                </div>
                <div class="stat-box">
                    <div class="stat-value">""" + str(stats['anorganik']) + """</div>
                    <div class="stat-label">Anorganik</div>
                </div>
            </div>
            
            <h2>🔌 API Endpoints</h2>
            <div class="endpoint">
                <strong>POST /upload</strong> - Upload image untuk inference
            </div>
            <div class="endpoint">
                <strong>GET /status</strong> - System status
            </div>
            <div class="endpoint">
                <strong>GET /stats</strong> - Statistics
            </div>
        </div>
    </body>
    </html>
    """

@app.route('/upload', methods=['POST'])
def upload_image():
    """
    Endpoint untuk receive image dari ESP32-CAM
    """
    start_time = time.time()
    
    print("\n" + "=" * 70)
    print("📨 RECEIVED IMAGE FROM ESP32-CAM")
    print("=" * 70)
    
    try:
        # Get image dari request
        if 'file' in request.files:
            file = request.files['file']
            image_bytes = file.read()
        else:
            image_bytes = request.data
        
        if len(image_bytes) == 0:
            return jsonify({
                'status': 'error',
                'message': 'No image data received'
            }), 400
        
        print(f"📷 Image size: {len(image_bytes)} bytes")
        
        # Convert bytes to image
        nparr = np.frombuffer(image_bytes, np.uint8)
        image = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        
        if image is None:
            return jsonify({
                'status': 'error',
                'message': 'Failed to decode image'
            }), 400
        
        print(f"   Image shape: {image.shape}")
        
        # Run inference
        result = run_inference(image, start_time)
        
        return jsonify(result), 200
        
    except Exception as e:
        print(f"❌ Error processing image: {e}")
        import traceback
        traceback.print_exc()
        return jsonify({
            'status': 'error',
            'message': str(e)
        }), 500

@app.route('/status', methods=['GET'])
def get_status():
    """
    Get system status
    """
    return jsonify({
        'status': 'running',
        'model_loaded': model is not None,
        'serial_connected': ser is not None,
        'stats': stats
    })

@app.route('/stats', methods=['GET'])
def get_stats():
    """
    Get statistics
    """
    return jsonify(stats)

# ========== INFERENCE ==========
def run_inference(image, start_time):
    """
    Run YOLOv8 inference dan kirim hasil ke ESP32
    """
    with processing_lock:
        print("\n🤖 Running YOLOv8 inference...")
        
        # Run YOLOv8
        results = model(image, conf=CONFIDENCE_THRESHOLD, verbose=False)
        
        # Get detections
        detections = results[0].boxes
        
        if len(detections) == 0:
            print("   ⚠️  No objects detected")
            return {
                'status': 'no_detection',
                'message': 'No waste detected',
                'class': -1
            }
        
        # Ambil detection dengan confidence tertinggi
        best_idx = detections.conf.argmax()
        predicted_class = int(detections.cls[best_idx])
        confidence = float(detections.conf[best_idx])
        
        class_name = CLASS_NAMES.get(predicted_class, "unknown")
        
        print(f"   ✅ Detection:")
        print(f"      Class: {predicted_class} ({class_name.upper()})")
        print(f"      Confidence: {confidence:.2%}")
        
        # Draw bounding box
        annotated_image = draw_results(image, results[0])
        
        # Save image
        if SAVE_IMAGES:
            save_detection_image(annotated_image, predicted_class, confidence)
        
        # Display GUI
        if SHOW_GUI:
            display_gui(annotated_image, predicted_class, confidence)
        
        # Send to ESP32
        success, comm_method = send_to_esp32(predicted_class)
        
        # Calculate latency
        latency_ms = (time.time() - start_time) * 1000
        
        # Update stats
        update_stats(predicted_class, latency_ms)
        
        # Log to CSV
        log_to_csv(predicted_class, class_name, confidence, success, comm_method, latency_ms)
        
        print(f"\n⏱️  Total latency: {latency_ms:.1f}ms")
        print("=" * 70)
        
        return {
            'status': 'success',
            'class': predicted_class,
            'class_name': class_name,
            'confidence': confidence,
            'communication': comm_method,
            'latency_ms': latency_ms
        }

def draw_results(image, result):
    """
    Draw bounding boxes dan labels di image
    """
    annotated = image.copy()
    
    for box in result.boxes:
        # Get box coordinates
        x1, y1, x2, y2 = box.xyxy[0].cpu().numpy().astype(int)
        cls = int(box.cls[0])
        conf = float(box.conf[0])
        
        # Get color
        color = CLASS_COLORS.get(cls, (255, 255, 255))
        
        # Draw box
        cv2.rectangle(annotated, (x1, y1), (x2, y2), color, 3)
        
        # Draw label
        label = f"{CLASS_NAMES.get(cls, 'unknown').upper()} {conf:.2%}"
        
        # Background untuk text
        (w, h), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.8, 2)
        cv2.rectangle(annotated, (x1, y1 - h - 10), (x1 + w, y1), color, -1)
        
        # Text
        cv2.putText(annotated, label, (x1, y1 - 5), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)
    
    return annotated

def save_detection_image(image, predicted_class, confidence):
    """
    Save image dengan detection results
    """
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    class_name = CLASS_NAMES.get(predicted_class, "unknown")
    filename = f"{timestamp}_{class_name}_{confidence:.2f}.jpg"
    filepath = os.path.join(IMAGE_SAVE_DIR, filename)
    
    cv2.imwrite(filepath, image)
    print(f"   💾 Image saved: {filename}")

def display_gui(image, predicted_class, confidence):
    """
    Display GUI dengan detection results
    """
    # Resize untuk display
    h, w = image.shape[:2]
    if w > 1280:
        scale = 1280 / w
        image = cv2.resize(image, None, fx=scale, fy=scale)
    
    # Add info text
    class_name = CLASS_NAMES.get(predicted_class, "unknown")
    info = f"CLASS: {class_name.upper()} | CONF: {confidence:.2%} | FPS: {stats['fps']:.1f}"
    
    cv2.putText(image, info, (10, 30), 
               cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
    
    # Show
    cv2.imshow("Sistem Pemilah Sampah - Detection", image)
    cv2.waitKey(1)

def send_to_esp32(predicted_class):
    """
    Kirim hasil klasifikasi ke ESP32 Main
    Primary: WiFi (HTTP)
    Fallback: Serial
    """
    print("\n📤 Sending result to ESP32...")
    
    # Try WiFi first
    success, method = send_via_wifi(predicted_class)
    
    if success:
        return True, method
    
    # Fallback ke Serial
    print("   ⚠️  WiFi failed, trying Serial...")
    success, method = send_via_serial(predicted_class)
    
    return success, method

def send_via_wifi(predicted_class):
    """
    Kirim via WiFi (HTTP POST)
    """
    try:
        url = f"http://{ESP32_MAIN_IP}:{ESP32_MAIN_PORT}/classify"
        payload = {'class': predicted_class}
        
        response = requests.post(url, json=payload, timeout=5)
        
        if response.status_code == 200:
            print(f"   ✅ Sent via WiFi to {ESP32_MAIN_IP}")
            return True, "WiFi"
        else:
            print(f"   ❌ WiFi failed: {response.status_code}")
            return False, "WiFi"
            
    except Exception as e:
        print(f"   ❌ WiFi error: {e}")
        return False, "WiFi"

def send_via_serial(predicted_class):
    """
    Kirim via Serial (fallback)
    """
    if ser is None:
        print("   ❌ Serial not available")
        return False, "Serial"
    
    try:
        message = f"{predicted_class}\n"
        ser.write(message.encode())
        print(f"   ✅ Sent via Serial: {predicted_class}")
        return True, "Serial"
        
    except Exception as e:
        print(f"   ❌ Serial error: {e}")
        return False, "Serial"

def update_stats(predicted_class, latency_ms):
    """
    Update statistics
    """
    global stats, last_process_time
    
    stats['total_processed'] += 1
    
    if predicted_class == 0:
        stats['organik'] += 1
    elif predicted_class == 1:
        stats['anorganik'] += 1
    elif predicted_class == 2:
        stats['b3'] += 1
    
    # Calculate FPS
    current_time = time.time()
    if last_process_time > 0:
        fps = 1.0 / (current_time - last_process_time)
        stats['fps'] = fps
    last_process_time = current_time
    
    # Update avg latency
    if stats['total_processed'] == 1:
        stats['avg_latency'] = latency_ms
    else:
        stats['avg_latency'] = (stats['avg_latency'] * (stats['total_processed'] - 1) + latency_ms) / stats['total_processed']

def log_to_csv(predicted_class, class_name, confidence, success, comm_method, latency_ms):
    """
    Log hasil ke CSV file
    """
    csv_path = os.path.join(LOG_DIR, LOG_FILE)
    
    with open(csv_path, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            datetime.now().isoformat(),
            predicted_class,
            class_name,
            f"{confidence:.4f}",
            "OK",  # bin_status (akan diupdate jika ada feedback dari ESP32)
            success,
            comm_method,
            f"{latency_ms:.1f}"
        ])

# ========== MAIN ==========
if __name__ == '__main__':
    # Initialize
    initialize()
    
    # Run Flask server
    try:
        app.run(
            host='0.0.0.0',
            port=LAPTOP_PORT,
            debug=DEBUG_MODE,
            threaded=True
        )
    except KeyboardInterrupt:
        print("\n\n🛑 Server stopped by user")
        if ser is not None:
            ser.close()
        cv2.destroyAllWindows()
        print("✅ Cleanup complete")
