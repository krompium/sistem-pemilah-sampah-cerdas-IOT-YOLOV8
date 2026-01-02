/*
=============================================================================
SISTEM PEMILAH SAMPAH CERDAS - ESP32-CAM MOTION CAPTURE
Modul ESP32-CAM untuk deteksi gerakan dan capture foto
=============================================================================

HARDWARE:
- ESP32-CAM AI Thinker
- Camera OV2640
- LED Flash (optional)

FITUR:
✅ Motion detection (auto trigger)
✅ High quality image capture
✅ HTTP server (streaming & capture endpoints)
✅ WiFi connection dengan auto-reconnect
✅ Send image to laptop via HTTP
✅ PSRAM optimization

WORKFLOW:
1. Deteksi gerakan (motion detection)
2. Capture foto berkualitas tinggi
3. Kirim foto ke laptop via WiFi
4. Laptop jalankan YOLOv8 inference
5. Ready untuk deteksi berikutnya

Author: @krompium
Untuk: UAS Sistem Pemilah Sampah Cerdas
=============================================================================
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"
#include "esp_timer.h"
#include "esp_http_server.h"

// ========== KONFIGURASI WIFI (GANTI SESUAI WIFI ANDA!) ==========
const char* ssid = "YourWiFiName";           // ⚙️ GANTI dengan nama WiFi Anda
const char* password = "YourPassword";        // ⚙️ GANTI dengan password WiFi Anda

// ========== KONFIGURASI LAPTOP (UNTUK KIRIM FOTO) ==========
const char* laptopIP = "192.168.1.100";      // ⚙️ GANTI dengan IP laptop Anda
const int laptopPort = 5000;                 // ⚙️ Port Flask server di laptop

// ========== KONFIGURASI MOTION DETECTION (CUSTOMIZABLE!) ==========
int MOTION_THRESHOLD = 30;          // ⚙️ Sensitivity (0-100, lebih kecil = lebih sensitif)
int CAPTURE_DELAY = 1000;           // ⚙️ Delay setelah motion detected (ms)
int MIN_INTERVAL = 3000;            // ⚙️ Interval minimum antar capture (ms)

// ========== KONFIGURASI CAMERA ==========
#define CAMERA_MODEL_AI_THINKER  // ESP32-CAM AI Thinker

// Pin definition untuk AI Thinker
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// LED Flash pin
#define LED_FLASH_PIN      4

// ========== GLOBAL VARIABLES ==========
httpd_handle_t camera_httpd = NULL;
bool wifiConnected = false;
bool cameraReady = false;
bool motionDetectionEnabled = true;

// Motion detection variables
camera_fb_t *lastFrame = NULL;
unsigned long lastCaptureTime = 0;
int motionCounter = 0;

// Statistics
int totalCaptures = 0;
int motionDetections = 0;

/*
=============================================================================
SETUP - INISIALISASI SISTEM
=============================================================================
*/
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("========================================");
  Serial.println("📷 SISTEM PEMILAH SAMPAH CERDAS");
  Serial.println("   ESP32-CAM Motion Capture v1.0");
  Serial.println("========================================\n");
  
  // Setup LED Flash
  pinMode(LED_FLASH_PIN, OUTPUT);
  digitalWrite(LED_FLASH_PIN, LOW);
  
  // Inisialisasi Camera
  if (setupCamera()) {
    cameraReady = true;
    Serial.println("✅ Camera ready!");
  } else {
    Serial.println("❌ Camera init FAILED!");
    return;
  }
  
  // Connect ke WiFi
  connectWiFi();
  
  // Start HTTP Server
  if (wifiConnected) {
    startCameraServer();
  }
  
  Serial.println("\n✅ SISTEM SIAP!");
  Serial.println("========================================");
  Serial.println("Motion detection: ENABLED");
  Serial.println("Waiting for motion...");
  Serial.println("========================================\n");
}

/*
=============================================================================
MAIN LOOP - LOOP UTAMA
=============================================================================
*/
void loop() {
  // Auto-reconnect WiFi jika terputus
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️  WiFi disconnected! Reconnecting...");
    wifiConnected = false;
    connectWiFi();
  }
  
  // Motion detection (jika diaktifkan)
  if (motionDetectionEnabled && cameraReady) {
    if (detectMotion()) {
      motionDetections++;
      Serial.println("\n🚨 MOTION DETECTED!");
      Serial.print("   Motion count: ");
      Serial.println(motionDetections);
      
      // Delay sebelum capture (beri waktu objek stabil)
      delay(CAPTURE_DELAY);
      
      // Capture dan kirim ke laptop
      captureAndSend();
    }
  }
  
  delay(100);  // Delay kecil untuk stabilitas
}

/*
=============================================================================
SETUP CAMERA - INISIALISASI CAMERA
=============================================================================
*/
bool setupCamera() {
  Serial.println("📷 Initializing camera...");
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Init dengan frame size tinggi jika ada PSRAM
  if (psramFound()) {
    Serial.println("   PSRAM found - using high quality settings");
    config.frame_size = FRAMESIZE_UXGA;    // 1600x1200
    config.jpeg_quality = 10;              // 0-63, lebih kecil = lebih baik
    config.fb_count = 2;
  } else {
    Serial.println("   No PSRAM - using standard quality settings");
    config.frame_size = FRAMESIZE_SVGA;    // 800x600
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("   Camera init failed with error 0x%x\n", err);
    return false;
  }
  
  // Sensor settings untuk kualitas lebih baik
  sensor_t *s = esp_camera_sensor_get();
  if (s != NULL) {
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_special_effect(s, 0); // 0 = No Effect
    s->set_whitebal(s, 1);       // 0 = disable, 1 = enable
    s->set_awb_gain(s, 1);       // 0 = disable, 1 = enable
    s->set_wb_mode(s, 0);        // 0 to 4
    s->set_exposure_ctrl(s, 1);  // 0 = disable, 1 = enable
    s->set_aec2(s, 0);           // 0 = disable, 1 = enable
    s->set_ae_level(s, 0);       // -2 to 2
    s->set_aec_value(s, 300);    // 0 to 1200
    s->set_gain_ctrl(s, 1);      // 0 = disable, 1 = enable
    s->set_agc_gain(s, 0);       // 0 to 30
    s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
    s->set_bpc(s, 0);            // 0 = disable, 1 = enable
    s->set_wpc(s, 1);            // 0 = disable, 1 = enable
    s->set_raw_gma(s, 1);        // 0 = disable, 1 = enable
    s->set_lenc(s, 1);           // 0 = disable, 1 = enable
    s->set_hmirror(s, 0);        // 0 = disable, 1 = enable
    s->set_vflip(s, 0);          // 0 = disable, 1 = enable
    s->set_dcw(s, 1);            // 0 = disable, 1 = enable
    s->set_colorbar(s, 0);       // 0 = disable, 1 = enable
  }
  
  Serial.println("   ✓ Camera initialized");
  return true;
}

/*
=============================================================================
CONNECT WIFI - KONEKSI KE WIFI
=============================================================================
*/
void connectWiFi() {
  Serial.println("📡 Connecting to WiFi...");
  Serial.print("   SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("   ✅ WiFi Connected!");
    Serial.print("   IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("   Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    wifiConnected = false;
    Serial.println("   ❌ WiFi connection FAILED!");
    Serial.println("   Will retry in background...");
  }
}

/*
=============================================================================
START CAMERA SERVER - START HTTP SERVER
=============================================================================
*/
void startCameraServer() {
  Serial.println("🌐 Starting camera HTTP server...");
  
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  
  httpd_uri_t capture_uri = {
    .uri       = "/capture",
    .method    = HTTP_GET,
    .handler   = capture_handler,
    .user_ctx  = NULL
  };
  
  httpd_uri_t status_uri = {
    .uri       = "/status",
    .method    = HTTP_GET,
    .handler   = status_handler,
    .user_ctx  = NULL
  };
  
  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };
  
  // Start server
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &capture_uri);
    httpd_register_uri_handler(camera_httpd, &status_uri);
    httpd_register_uri_handler(camera_httpd, &stream_uri);
    
    Serial.println("   ✓ HTTP server started on port 80");
    Serial.println("   Endpoints:");
    Serial.println("   - GET /capture (single image)");
    Serial.println("   - GET /stream (MJPEG stream)");
    Serial.println("   - GET /status (system status)");
  } else {
    Serial.println("   ❌ HTTP server failed to start");
  }
}

/*
=============================================================================
CAPTURE HANDLER - HANDLER UNTUK ENDPOINT /capture
=============================================================================
*/
static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  
  // Capture image
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  // Send image
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  
  // Return frame buffer
  esp_camera_fb_return(fb);
  
  return res;
}

/*
=============================================================================
STATUS HANDLER - HANDLER UNTUK ENDPOINT /status
=============================================================================
*/
static esp_err_t status_handler(httpd_req_t *req) {
  static char json_response[256];
  
  char *p = json_response;
  *p++ = '{';
  
  p += sprintf(p, "\"wifi_connected\":%s,", wifiConnected ? "true" : "false");
  p += sprintf(p, "\"camera_ready\":%s,", cameraReady ? "true" : "false");
  p += sprintf(p, "\"motion_enabled\":%s,", motionDetectionEnabled ? "true" : "false");
  p += sprintf(p, "\"total_captures\":%d,", totalCaptures);
  p += sprintf(p, "\"motion_detections\":%d,", motionDetections);
  p += sprintf(p, "\"rssi\":%d", WiFi.RSSI());
  
  *p++ = '}';
  *p++ = 0;
  
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json_response, strlen(json_response));
}

/*
=============================================================================
STREAM HANDLER - HANDLER UNTUK ENDPOINT /stream (MJPEG)
=============================================================================
*/
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[64];
  
  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) {
    return res;
  }
  
  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if (fb->format != PIXFORMAT_JPEG) {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) {
          Serial.println("JPEG compression failed");
          res = ESP_FAIL;
        }
      } else {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
      }
    }
    
    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    
    if (res != ESP_OK) {
      break;
    }
  }
  
  return res;
}

/*
=============================================================================
DETECT MOTION - DETEKSI GERAKAN
=============================================================================
Membandingkan frame saat ini dengan frame sebelumnya.
Jika perbedaan > threshold, dianggap ada motion.
=============================================================================
*/
bool detectMotion() {
  // Cek interval minimum
  if (millis() - lastCaptureTime < MIN_INTERVAL) {
    return false;
  }
  
  // Capture frame saat ini
  camera_fb_t *currentFrame = esp_camera_fb_get();
  if (!currentFrame) {
    return false;
  }
  
  // Jika ini frame pertama, simpan dan return false
  if (lastFrame == NULL) {
    lastFrame = currentFrame;
    return false;
  }
  
  // Hitung perbedaan antara frame
  bool motionDetected = false;
  
  // Simplified motion detection: bandingkan ukuran file JPEG
  // Frame dengan motion biasanya punya ukuran lebih besar karena detail lebih banyak
  int sizeDiff = abs((int)currentFrame->len - (int)lastFrame->len);
  float diffPercent = (float)sizeDiff / (float)lastFrame->len * 100.0;
  
  if (diffPercent > MOTION_THRESHOLD) {
    motionDetected = true;
  }
  
  // Untuk deteksi lebih akurat, bandingkan pixel (membutuhkan lebih banyak CPU)
  // Uncomment jika ingin deteksi lebih akurat tapi lebih lambat
  /*
  if (currentFrame->len == lastFrame->len) {
    int diffCount = 0;
    int sampleInterval = 100;  // Sample setiap 100 byte
    
    for (size_t i = 0; i < currentFrame->len; i += sampleInterval) {
      if (abs(currentFrame->buf[i] - lastFrame->buf[i]) > 30) {
        diffCount++;
      }
    }
    
    float diffRatio = (float)diffCount / (currentFrame->len / sampleInterval) * 100.0;
    if (diffRatio > MOTION_THRESHOLD) {
      motionDetected = true;
    }
  }
  */
  
  // Return old frame, simpan current frame
  esp_camera_fb_return(lastFrame);
  lastFrame = currentFrame;
  
  return motionDetected;
}

/*
=============================================================================
CAPTURE AND SEND - CAPTURE FOTO DAN KIRIM KE LAPTOP
=============================================================================
*/
void captureAndSend() {
  Serial.println("\n📸 Capturing image...");
  
  // Flash LED (optional, untuk lighting lebih baik)
  digitalWrite(LED_FLASH_PIN, HIGH);
  delay(100);
  
  // Capture image
  camera_fb_t *fb = esp_camera_fb_get();
  
  digitalWrite(LED_FLASH_PIN, LOW);
  
  if (!fb) {
    Serial.println("   ❌ Camera capture failed");
    return;
  }
  
  Serial.print("   Image size: ");
  Serial.print(fb->len);
  Serial.println(" bytes");
  
  // Kirim ke laptop via HTTP POST
  if (wifiConnected) {
    sendImageToLaptop(fb);
  } else {
    Serial.println("   ⚠️  WiFi not connected, cannot send image");
  }
  
  // Return frame buffer
  esp_camera_fb_return(fb);
  
  totalCaptures++;
  lastCaptureTime = millis();
  
  Serial.println("   ✓ Capture complete");
}

/*
=============================================================================
SEND IMAGE TO LAPTOP - KIRIM IMAGE KE LAPTOP VIA HTTP
=============================================================================
*/
void sendImageToLaptop(camera_fb_t *fb) {
  Serial.println("   📤 Sending to laptop...");
  
  HTTPClient http;
  
  String url = "http://" + String(laptopIP) + ":" + String(laptopPort) + "/upload";
  
  Serial.print("   URL: ");
  Serial.println(url);
  
  http.begin(url);
  http.addHeader("Content-Type", "image/jpeg");
  
  int httpResponseCode = http.POST(fb->buf, fb->len);
  
  if (httpResponseCode > 0) {
    Serial.print("   ✅ Sent! Response: ");
    Serial.println(httpResponseCode);
    
    String response = http.getString();
    Serial.print("   Server response: ");
    Serial.println(response);
  } else {
    Serial.print("   ❌ Send failed! Error: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

/*
=============================================================================
HELPER FUNCTIONS
=============================================================================
*/

// Convert frame to JPEG (jika diperlukan)
bool frame2jpg(camera_fb_t *fb, int quality, uint8_t **jpg_buf, size_t *jpg_buf_len) {
  if (fb->format == PIXFORMAT_JPEG) {
    *jpg_buf = fb->buf;
    *jpg_buf_len = fb->len;
    return true;
  }
  
  // Conversion diperlukan
  bool success = false;
  
  // Allocate buffer
  *jpg_buf = (uint8_t *)malloc(fb->len);
  if (*jpg_buf == NULL) {
    return false;
  }
  
  // Convert (implementasi tergantung format)
  // Untuk simplifikasi, return false jika bukan JPEG
  free(*jpg_buf);
  *jpg_buf = NULL;
  return false;
}
