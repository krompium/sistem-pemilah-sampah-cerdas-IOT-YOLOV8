"""
=============================================================================
SISTEM PEMILAH SAMPAH CERDAS - CONFIGURATION FILE
File konfigurasi terpusat untuk semua parameter sistem
=============================================================================

File ini berisi semua konfigurasi yang dapat disesuaikan:
- WiFi credentials
- IP addresses
- Blynk authentication
- Servo positions
- Sensor thresholds
- Model settings
- Class mappings

CARA PAKAI:
1. Copy file ini dan rename menjadi config.py
2. Sesuaikan nilai-nilai di bawah dengan setup Anda
3. Import di script Python: from config import *

Author: @krompium
Untuk: UAS Sistem Pemilah Sampah Cerdas
=============================================================================
"""

# ========== WIFI SETTINGS ==========
WIFI_SSID = "YourWiFiName"                    # ⚙️ GANTI dengan nama WiFi Anda
WIFI_PASSWORD = "YourPassword"                 # ⚙️ GANTI dengan password WiFi

# ========== IP ADDRESSES ==========
ESP32CAM_IP = "192.168.1.101"                 # ⚙️ IP ESP32-CAM (set di router atau DHCP)
ESP32_MAIN_IP = "192.168.1.102"               # ⚙️ IP ESP32 Main Controller
LAPTOP_IP = "192.168.1.100"                   # ⚙️ IP Laptop (untuk receive dari ESP32-CAM)

# Port configurations
ESP32CAM_PORT = 80
ESP32_MAIN_PORT = 80
LAPTOP_PORT = 5000                            # Flask server port

# ========== BLYNK IOT ==========
BLYNK_AUTH = "YourBlynkAuthToken"             # ⚙️ GANTI dengan Blynk Auth Token
BLYNK_SERVER = "blynk.cloud"                  # Blynk server (default: blynk.cloud)
BLYNK_PORT = 80

# Virtual Pins
BLYNK_VPIN_ORGANIK = "V0"                     # Counter organik
BLYNK_VPIN_ANORGANIK = "V1"                   # Counter anorganik
BLYNK_VPIN_B3 = "V2"                          # Counter B3
BLYNK_VPIN_TOTAL = "V3"                       # Total processed
BLYNK_VPIN_STATUS = "V4"                      # System status
BLYNK_VPIN_BIN_STATUS = "V5"                  # Bin status (JSON)
BLYNK_VPIN_IMAGE = "V6"                       # Last captured image
BLYNK_VPIN_RESET = "V7"                       # Reset button

# ========== SERVO PENADAH (CUSTOMIZABLE!) ==========
SERVO_PENADAH_CLOSE = 0                       # ⚙️ Sudut untuk TUTUP penadah (0-180)
SERVO_PENADAH_OPEN = 90                       # ⚙️ Sudut untuk BUKA penadah (0-180)
SERVO_PENADAH_DELAY = 2000                    # ⚙️ Delay setelah buka (ms)

# ========== SERVO PLATFORM (CUSTOMIZABLE!) ==========
SERVO_PLATFORM_HOME = 0                       # ⚙️ Posisi HOME
SERVO_PLATFORM_ORGANIK = 0                    # ⚙️ Posisi untuk bin Organik
SERVO_PLATFORM_ANORGANIK = 90                 # ⚙️ Posisi untuk bin Anorganik
SERVO_PLATFORM_B3 = 180                       # ⚙️ Posisi untuk bin B3
SERVO_PLATFORM_DELAY = 500                    # ⚙️ Delay setelah rotasi (ms)

# ========== ULTRASONIC THRESHOLDS (CUSTOMIZABLE!) ==========
THRESHOLD_ORGANIK_FULL = 5                    # ⚙️ Jarak (cm) dianggap FULL
THRESHOLD_ANORGANIK_FULL = 5                  # ⚙️ Jarak (cm) dianggap FULL
THRESHOLD_B3_FULL = 5                         # ⚙️ Jarak (cm) dianggap FULL

# ========== MODEL SETTINGS ==========
MODEL_PATH = "models/best.pt"                 # ⚙️ Path ke YOLOv8 model
CONFIDENCE_THRESHOLD = 0.70                   # ⚙️ Confidence threshold (0.0-1.0)
IOU_THRESHOLD = 0.45                          # ⚙️ IoU threshold untuk NMS

# Image settings
IMAGE_SIZE = 640                              # Input size untuk YOLOv8 (640 recommended)
MAX_DETECTIONS = 10                           # Max detections per image

# ========== CLASS MAPPING ==========
# Mapping dari class index ke nama kategori
CLASS_NAMES = {
    0: "organik",
    1: "anorganik", 
    2: "b3"
}

# Mapping dari nama ke class index (reverse)
CLASS_INDICES = {
    "organik": 0,
    "anorganik": 1,
    "b3": 2
}

# Class colors untuk visualisasi (BGR format)
CLASS_COLORS = {
    0: (0, 255, 0),      # Green untuk Organik
    1: (255, 0, 0),      # Blue untuk Anorganik
    2: (0, 0, 255)       # Red untuk B3
}

# ========== SERIAL COMMUNICATION ==========
# Backup communication jika WiFi gagal
SERIAL_PORT_WINDOWS = "COM3"                  # ⚙️ Port untuk Windows
SERIAL_PORT_LINUX = "/dev/ttyUSB0"            # ⚙️ Port untuk Linux
SERIAL_BAUD_RATE = 115200
SERIAL_TIMEOUT = 2                            # seconds

# ========== MOTION DETECTION ==========
MOTION_THRESHOLD = 30                         # ⚙️ Sensitivity (0-100)
CAPTURE_DELAY = 1000                          # ⚙️ Delay setelah motion (ms)
MIN_CAPTURE_INTERVAL = 3000                   # ⚙️ Min interval antar capture (ms)

# ========== LOGGING ==========
LOG_DIR = "logs"                              # Directory untuk log files
LOG_FILE = "waste_sorting.csv"                # CSV log file
SAVE_IMAGES = True                            # Save captured images?
IMAGE_SAVE_DIR = "captured_images"            # Directory untuk save images

# CSV columns
CSV_COLUMNS = [
    "timestamp",
    "predicted_class",
    "class_name",
    "confidence",
    "bin_status",
    "success"
]

# ========== PERFORMANCE ==========
USE_CUDA = True                               # Use GPU jika tersedia
USE_HALF_PRECISION = False                    # FP16 inference (faster, sedikit kurang akurat)
NUM_THREADS = 4                               # CPU threads (jika tidak pakai GPU)

# ========== TIMEOUTS ==========
HTTP_TIMEOUT = 10                             # HTTP request timeout (seconds)
WIFI_RECONNECT_INTERVAL = 30                  # WiFi reconnect interval (seconds)
BLYNK_RECONNECT_INTERVAL = 60                 # Blynk reconnect interval (seconds)

# ========== GUI SETTINGS ==========
SHOW_GUI = True                               # Show detection GUI window
GUI_WINDOW_NAME = "Sistem Pemilah Sampah - Live Detection"
GUI_WINDOW_WIDTH = 1280
GUI_WINDOW_HEIGHT = 720
DISPLAY_FPS = True                            # Show FPS counter
DISPLAY_CONFIDENCE = True                     # Show confidence score

# ========== NOTIFICATIONS ==========
ENABLE_SOUND = False                          # Play sound saat detection
SOUND_FILE = "notification.wav"               # Sound file path

# Email notification (optional)
ENABLE_EMAIL = False
EMAIL_SMTP = "smtp.gmail.com"
EMAIL_PORT = 587
EMAIL_USER = "your_email@gmail.com"
EMAIL_PASSWORD = "your_app_password"
EMAIL_TO = "notification@example.com"

# ========== DEBUG ==========
DEBUG_MODE = True                             # Print debug information
VERBOSE_LOGGING = True                        # Detailed logs
SAVE_DEBUG_IMAGES = False                     # Save images dengan bounding boxes

# ========== SYSTEM ==========
AUTO_START = True                             # Auto start detection saat script run
ENABLE_FALLBACK = True                        # Enable Serial fallback jika WiFi gagal
MAX_RETRY_ATTEMPTS = 3                        # Max retry untuk HTTP requests

# ========== CALIBRATION ==========
CALIBRATION_MODE = False                      # Enable calibration mode
CALIBRATION_SAMPLES = 10                      # Number of samples untuk calibration

# ========== ADVANCED ==========
# YOLOv8 advanced settings
YOLO_DEVICE = "0"                             # "0" for GPU, "cpu" for CPU
YOLO_VERBOSE = False                          # YOLOv8 verbose output
YOLO_AGNOSTIC_NMS = False                     # Class-agnostic NMS
YOLO_MAX_DET = 10                             # Maximum detections

# Frame processing
SKIP_FRAMES = 0                               # Skip N frames (untuk speed up)
RESIZE_BEFORE_INFERENCE = False               # Resize image sebelum inference

# ========== VALIDATION ==========
def validate_config():
    """
    Validasi konfigurasi
    Memastikan semua nilai valid dan konsisten
    """
    errors = []
    
    # Validasi WiFi
    if WIFI_SSID == "YourWiFiName":
        errors.append("⚠️  WiFi SSID belum diubah!")
    
    # Validasi Blynk
    if BLYNK_AUTH == "YourBlynkAuthToken":
        errors.append("⚠️  Blynk Auth Token belum diubah!")
    
    # Validasi IP addresses
    if ESP32CAM_IP == "192.168.1.101":
        errors.append("⚠️  ESP32-CAM IP masih default (mungkin perlu diubah)")
    
    # Validasi model path
    import os
    if not os.path.exists(MODEL_PATH):
        errors.append(f"⚠️  Model file tidak ditemukan: {MODEL_PATH}")
    
    # Validasi thresholds
    if CONFIDENCE_THRESHOLD < 0 or CONFIDENCE_THRESHOLD > 1:
        errors.append("❌ CONFIDENCE_THRESHOLD harus 0.0-1.0")
    
    if len(errors) > 0:
        print("\n⚠️  PERINGATAN KONFIGURASI:")
        print("=" * 50)
        for error in errors:
            print(f"  {error}")
        print("=" * 50)
        print("\nSilakan periksa dan update config.py\n")
        return False
    
    print("✅ Konfigurasi valid!")
    return True

# ========== PRINT CONFIG ==========
def print_config():
    """
    Print konfigurasi saat ini
    """
    print("\n" + "=" * 60)
    print("⚙️  KONFIGURASI SISTEM PEMILAH SAMPAH CERDAS")
    print("=" * 60)
    
    print("\n📡 NETWORK:")
    print(f"  WiFi SSID       : {WIFI_SSID}")
    print(f"  ESP32-CAM IP    : {ESP32CAM_IP}:{ESP32CAM_PORT}")
    print(f"  ESP32 Main IP   : {ESP32_MAIN_IP}:{ESP32_MAIN_PORT}")
    print(f"  Laptop IP       : {LAPTOP_IP}:{LAPTOP_PORT}")
    
    print("\n☁️  BLYNK:")
    print(f"  Server          : {BLYNK_SERVER}")
    print(f"  Auth Token      : {BLYNK_AUTH[:8]}..." if BLYNK_AUTH != "YourBlynkAuthToken" else "  Auth Token      : NOT SET")
    
    print("\n🔧 SERVO POSITIONS:")
    print(f"  Penadah CLOSE   : {SERVO_PENADAH_CLOSE}°")
    print(f"  Penadah OPEN    : {SERVO_PENADAH_OPEN}°")
    print(f"  Platform HOME   : {SERVO_PLATFORM_HOME}°")
    print(f"  Platform Organik: {SERVO_PLATFORM_ORGANIK}°")
    print(f"  Platform Anorganik: {SERVO_PLATFORM_ANORGANIK}°")
    print(f"  Platform B3     : {SERVO_PLATFORM_B3}°")
    
    print("\n📏 SENSOR THRESHOLDS:")
    print(f"  Organik FULL    : {THRESHOLD_ORGANIK_FULL} cm")
    print(f"  Anorganik FULL  : {THRESHOLD_ANORGANIK_FULL} cm")
    print(f"  B3 FULL         : {THRESHOLD_B3_FULL} cm")
    
    print("\n🤖 MODEL:")
    print(f"  Path            : {MODEL_PATH}")
    print(f"  Confidence      : {CONFIDENCE_THRESHOLD}")
    print(f"  Device          : {YOLO_DEVICE}")
    
    print("\n📊 CLASSES:")
    for idx, name in CLASS_NAMES.items():
        print(f"  {idx} = {name.upper()}")
    
    print("\n" + "=" * 60 + "\n")

# Auto-run validation jika diimport
if __name__ == "__main__":
    print_config()
    validate_config()
