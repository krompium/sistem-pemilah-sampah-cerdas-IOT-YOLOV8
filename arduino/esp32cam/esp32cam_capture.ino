/*
 * SISTEM PEMILAH SAMPAH CERDAS - ESP32-CAM
 * =========================================
 * 
 * ESP32-CAM untuk capture gambar sampah
 * 
 * Features:
 * - Capture foto saat trigger dari ESP32 main
 * - Kirim gambar via WiFi ke laptop
 * - LED flash indicator
 * 
 * Hardware:
 * - ESP32-CAM AI Thinker
 * - Camera OV2640 2MP
 * 
 * Pin Configuration:
 * - IO0: Boot mode (GND saat upload)
 * - IO4: Flash LED
 * - Serial: RX/TX untuk komunikasi
 * 
 * Upload Instructions:
 * 1. Connect IO0 to GND
 * 2. Upload sketch
 * 3. Disconnect IO0 from GND
 * 4. Reset ESP32-CAM
 */

#include "esp_camera.h"
#include <WiFi.h>

// Camera pins for AI Thinker ESP32-CAM
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

// Flash LED
#define FLASH_LED_PIN 4

// WiFi credentials (OPTIONAL - untuk remote capture)
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {
  Serial.begin(115200);
  Serial.println("========================================");
  Serial.println("SISTEM PEMILAH SAMPAH CERDAS");
  Serial.println("ESP32-CAM Module");
  Serial.println("========================================");
  
  // Setup flash LED
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, LOW);
  
  // Camera configuration
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
  
  // Frame size
  if(psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;  // 1600x1200
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;  // 800x600
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Camera init failed: 0x%x\n", err);
    return;
  }
  
  Serial.println("✅ Camera initialized!");
  
  // Test flash
  testFlash();
  
  Serial.println("\n⏳ Ready untuk capture...\n");
}

void loop() {
  // Check for capture command from Serial
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "CAM:CAPTURE") {
      captureAndSend();
    }
  }
  
  delay(100);
}

void captureAndSend() {
  Serial.println("📸 Capturing image...");
  
  // Turn on flash
  digitalWrite(FLASH_LED_PIN, HIGH);
  delay(100);
  
  // Capture photo
  camera_fb_t * fb = esp_camera_fb_get();
  
  // Turn off flash
  digitalWrite(FLASH_LED_PIN, LOW);
  
  if (!fb) {
    Serial.println("❌ Camera capture failed!");
    return;
  }
  
  Serial.printf("✅ Image captured: %d bytes\n", fb->len);
  
  // TODO: Send image to laptop via WiFi/Serial
  // For now, just notify
  Serial.println("CAM:IMAGE_READY");
  
  // Return frame buffer
  esp_camera_fb_return(fb);
}

void testFlash() {
  Serial.println("🔦 Testing flash...");
  
  for (int i = 0; i < 3; i++) {
    digitalWrite(FLASH_LED_PIN, HIGH);
    delay(200);
    digitalWrite(FLASH_LED_PIN, LOW);
    delay(200);
  }
  
  Serial.println("✅ Flash test selesai!");
}
