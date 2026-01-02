/*
=============================================================================
SISTEM PEMILAH SAMPAH CERDAS - ESP32 MAIN CONTROLLER
Kontroller utama untuk sistem pemilah sampah otomatis berbasis IoT
=============================================================================

HARDWARE:
- ESP32 DevKit V1
- 2x Servo (SG90/MG996R) - Penadah & Platform
- 3x HC-SR04 Ultrasonic - Deteksi bin full
- 5x LED - Indikator status

FITUR:
✅ 2 Servo control (penadah + platform rotasi)
✅ 3 Sensor ultrasonik (cek bin penuh)
✅ WiFi server (terima hasil klasifikasi)
✅ Serial communication (backup)
✅ Blynk IoT integration
✅ LED indicators
✅ Mode kalibrasi
✅ Error handling

WORKFLOW:
1. Terima hasil klasifikasi dari laptop (WiFi/Serial)
2. Cek bin tujuan penuh atau tidak
3. Putar platform ke posisi yang sesuai
4. Buka penadah untuk jatuhkan sampah
5. Tutup penadah
6. Platform kembali ke HOME
7. Update counter & Blynk

Author: @krompium
Untuk: UAS Sistem Pemilah Sampah Cerdas
=============================================================================
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <BlynkSimpleEsp32.h>

// ========== KONFIGURASI WIFI (GANTI SESUAI WIFI ANDA!) ==========
const char* ssid = "YourWiFiName";           // ⚙️ GANTI dengan nama WiFi Anda
const char* password = "YourPassword";        // ⚙️ GANTI dengan password WiFi Anda

// ========== KONFIGURASI BLYNK (GANTI DENGAN TOKEN ANDA!) ==========
char auth[] = "YourBlynkAuthToken";          // ⚙️ GANTI dengan Blynk Auth Token Anda

// ========== PIN DEFINITIONS ==========
// Servo Pins
#define SERVO_PENADAH_PIN    12  // Servo untuk buka/tutup penadah sementara
#define SERVO_PLATFORM_PIN   13  // Servo untuk putar platform ke 3 bin

// Ultrasonic Sensor - Bin Organik
#define TRIG_ORGANIK   26
#define ECHO_ORGANIK   27

// Ultrasonic Sensor - Bin Anorganik
#define TRIG_ANORGANIK 14
#define ECHO_ANORGANIK 25

// Ultrasonic Sensor - Bin B3
#define TRIG_B3        32
#define ECHO_B3        33

// LED Indicators
#define LED_ORGANIK    18  // LED Hijau - Organik
#define LED_ANORGANIK  19  // LED Biru - Anorganik
#define LED_B3         21  // LED Merah - B3
#define LED_SYSTEM     22  // LED Putih - System OK
#define LED_ERROR      23  // LED Kuning - Error/Full

// ========== KONFIGURASI SERVO PENADAH (CUSTOMIZABLE!) ==========
int SERVO_PENADAH_CLOSE = 0;        // ⚙️ Sudut untuk TUTUP penadah (0-180)
int SERVO_PENADAH_OPEN = 90;        // ⚙️ Sudut untuk BUKA penadah (0-180)
int SERVO_PENADAH_DELAY = 2000;     // ⚙️ Delay setelah buka (ms)

// ========== KONFIGURASI SERVO PLATFORM (CUSTOMIZABLE!) ==========
int SERVO_PLATFORM_HOME = 0;        // ⚙️ Posisi HOME platform
int SERVO_PLATFORM_ORGANIK = 0;     // ⚙️ Posisi platform untuk bin Organik
int SERVO_PLATFORM_ANORGANIK = 90;  // ⚙️ Posisi platform untuk bin Anorganik
int SERVO_PLATFORM_B3 = 180;        // ⚙️ Posisi platform untuk bin B3
int SERVO_PLATFORM_DELAY = 500;     // ⚙️ Delay setelah rotasi (ms)

// ========== KONFIGURASI ULTRASONIC (CUSTOMIZABLE!) ==========
int THRESHOLD_ORGANIK_FULL = 5;     // ⚙️ Jarak (cm) dianggap bin FULL
int THRESHOLD_ANORGANIK_FULL = 5;   // ⚙️ Jarak (cm) dianggap bin FULL
int THRESHOLD_B3_FULL = 5;          // ⚙️ Jarak (cm) dianggap bin FULL

// ========== SISTEM SETTINGS ==========
#define SERIAL_BAUD     115200
#define ULTRASONIC_TIMEOUT 30000  // Timeout untuk ultrasonic (microseconds)
#define WIFI_TIMEOUT    20000     // Timeout WiFi connection (ms)
#define BLYNK_TIMEOUT   10000     // Timeout Blynk connection (ms)

// ========== GLOBAL VARIABLES ==========
Servo servoPenadah;     // Servo untuk buka/tutup penadah
Servo servoPlatform;    // Servo untuk rotasi platform

WebServer server(80);   // HTTP server di port 80

// Counters untuk setiap kategori sampah
int counterOrganik = 0;
int counterAnorganik = 0;
int counterB3 = 0;
int totalProcessed = 0;

// Status bin (true = FULL, false = KOSONG)
bool binOrganikFull = false;
bool binAnorganikFull = false;
bool binB3Full = false;

// Status sistem
bool wifiConnected = false;
bool blynkConnected = false;
bool systemReady = false;

// Buffer untuk komunikasi
String receivedData = "";
int lastClassification = -1;

// Blynk Timer
BlynkTimer timer;

// ========== VIRTUAL PINS BLYNK ==========
#define VPIN_ORGANIK    V0  // Counter Organik
#define VPIN_ANORGANIK  V1  // Counter Anorganik
#define VPIN_B3         V2  // Counter B3
#define VPIN_TOTAL      V3  // Total processed
#define VPIN_STATUS     V4  // System status (string)
#define VPIN_BIN_STATUS V5  // Bin full status (JSON)
#define VPIN_RESET      V6  // Reset button

/*
=============================================================================
SETUP - INISIALISASI SISTEM
=============================================================================
*/
void setup() {
  // Inisialisasi Serial untuk debugging
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("========================================");
  Serial.println("🗑️  SISTEM PEMILAH SAMPAH CERDAS");
  Serial.println("    ESP32 Main Controller v1.0");
  Serial.println("========================================\n");
  
  // Setup Pin Modes
  setupPins();
  
  // Setup Servo
  setupServos();
  
  // Test LED (semua nyala sebentar)
  testLEDs();
  
  // Connect ke WiFi
  connectWiFi();
  
  // Setup Web Server
  setupWebServer();
  
  // Connect ke Blynk (jika WiFi tersambung)
  if (wifiConnected) {
    connectBlynk();
  }
  
  // Setup Blynk Timer
  timer.setInterval(5000L, sendDataToBlynk);  // Update Blynk setiap 5 detik
  
  // Kalibrasi mode (jika diperlukan - uncomment untuk test)
  // calibrationMode();
  
  // System Ready!
  systemReady = true;
  digitalWrite(LED_SYSTEM, HIGH);
  
  Serial.println("\n✅ SISTEM SIAP!");
  Serial.println("========================================");
  Serial.println("Menunggu input klasifikasi...");
  Serial.println("Format Serial: 0=Organik, 1=Anorganik, 2=B3");
  Serial.println("Format HTTP: POST /classify dengan body: {\"class\":0}");
  Serial.println("========================================\n");
}

/*
=============================================================================
MAIN LOOP - LOOP UTAMA SISTEM
=============================================================================
*/
void loop() {
  // Handle Web Server requests
  if (wifiConnected) {
    server.handleClient();
  }
  
  // Handle Blynk
  if (blynkConnected) {
    Blynk.run();
    timer.run();
  }
  
  // Handle Serial Input (backup communication)
  if (Serial.available()) {
    handleSerialInput();
  }
  
  // Update sensor status setiap loop
  checkAllBins();
  
  // Blink system LED untuk tanda sistem hidup
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 1000) {
    digitalWrite(LED_SYSTEM, !digitalRead(LED_SYSTEM));
    lastBlink = millis();
  }
  
  delay(10);  // Delay kecil untuk stabilitas
}

/*
=============================================================================
PIN SETUP - INISIALISASI SEMUA PIN
=============================================================================
*/
void setupPins() {
  Serial.println("📌 Setting up pins...");
  
  // LED Pins sebagai OUTPUT
  pinMode(LED_ORGANIK, OUTPUT);
  pinMode(LED_ANORGANIK, OUTPUT);
  pinMode(LED_B3, OUTPUT);
  pinMode(LED_SYSTEM, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);
  
  // Matikan semua LED
  digitalWrite(LED_ORGANIK, LOW);
  digitalWrite(LED_ANORGANIK, LOW);
  digitalWrite(LED_B3, LOW);
  digitalWrite(LED_SYSTEM, LOW);
  digitalWrite(LED_ERROR, LOW);
  
  // Ultrasonic Pins
  pinMode(TRIG_ORGANIK, OUTPUT);
  pinMode(ECHO_ORGANIK, INPUT);
  pinMode(TRIG_ANORGANIK, OUTPUT);
  pinMode(ECHO_ANORGANIK, INPUT);
  pinMode(TRIG_B3, OUTPUT);
  pinMode(ECHO_B3, INPUT);
  
  Serial.println("   ✓ Pins configured");
}

/*
=============================================================================
SERVO SETUP - INISIALISASI SERVO
=============================================================================
*/
void setupServos() {
  Serial.println("🔧 Setting up servos...");
  
  // Attach servo ke pin
  servoPenadah.attach(SERVO_PENADAH_PIN);
  servoPlatform.attach(SERVO_PLATFORM_PIN);
  
  // Set ke posisi awal
  servoPenadah.write(SERVO_PENADAH_CLOSE);
  servoPlatform.write(SERVO_PLATFORM_HOME);
  
  delay(500);  // Tunggu servo ke posisi
  
  Serial.println("   ✓ Servos ready");
  Serial.println("   - Penadah: CLOSED");
  Serial.println("   - Platform: HOME");
}

/*
=============================================================================
TEST LED - TEST SEMUA LED
=============================================================================
*/
void testLEDs() {
  Serial.println("💡 Testing LEDs...");
  
  // Nyalakan semua LED
  digitalWrite(LED_ORGANIK, HIGH);
  digitalWrite(LED_ANORGANIK, HIGH);
  digitalWrite(LED_B3, HIGH);
  digitalWrite(LED_SYSTEM, HIGH);
  digitalWrite(LED_ERROR, HIGH);
  delay(500);
  
  // Matikan semua LED
  digitalWrite(LED_ORGANIK, LOW);
  digitalWrite(LED_ANORGANIK, LOW);
  digitalWrite(LED_B3, LOW);
  digitalWrite(LED_SYSTEM, LOW);
  digitalWrite(LED_ERROR, LOW);
  
  Serial.println("   ✓ LEDs OK");
}

/*
=============================================================================
WIFI CONNECTION - KONEKSI KE WIFI
=============================================================================
*/
void connectWiFi() {
  Serial.println("📡 Connecting to WiFi...");
  Serial.print("   SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  unsigned long startAttempt = millis();
  
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startAttempt < WIFI_TIMEOUT) {
    delay(500);
    Serial.print(".");
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
    Serial.println("   ⚠️  WiFi connection FAILED!");
    Serial.println("   System will use SERIAL communication only");
  }
}

/*
=============================================================================
WEB SERVER SETUP - SETUP HTTP SERVER
=============================================================================
*/
void setupWebServer() {
  if (!wifiConnected) return;
  
  Serial.println("🌐 Setting up web server...");
  
  // Endpoint untuk terima klasifikasi dari laptop
  server.on("/classify", HTTP_POST, handleClassification);
  
  // Endpoint untuk cek status sistem
  server.on("/status", HTTP_GET, handleStatus);
  
  // Endpoint untuk reset counter
  server.on("/reset", HTTP_GET, handleReset);
  
  // Start server
  server.begin();
  
  Serial.println("   ✓ Web server started on port 80");
  Serial.println("   Endpoints:");
  Serial.println("   - POST /classify (body: {\"class\":0})");
  Serial.println("   - GET  /status");
  Serial.println("   - GET  /reset");
}

/*
=============================================================================
BLYNK CONNECTION - KONEKSI KE BLYNK
=============================================================================
*/
void connectBlynk() {
  Serial.println("☁️  Connecting to Blynk...");
  
  Blynk.config(auth);
  
  unsigned long startAttempt = millis();
  
  while (!Blynk.connect() && 
         millis() - startAttempt < BLYNK_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  
  if (Blynk.connected()) {
    blynkConnected = true;
    Serial.println("   ✅ Blynk Connected!");
  } else {
    blynkConnected = false;
    Serial.println("   ⚠️  Blynk connection FAILED!");
    Serial.println("   Dashboard monitoring disabled");
  }
}

/*
=============================================================================
HANDLE CLASSIFICATION - TERIMA & PROSES HASIL KLASIFIKASI
=============================================================================
*/
void handleClassification() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "ERROR: No data received");
    return;
  }
  
  String body = server.arg("plain");
  Serial.println("\n📨 Received classification via HTTP:");
  Serial.println("   Data: " + body);
  
  // Parse JSON sederhana (format: {"class":0})
  int classIdx = -1;
  int startPos = body.indexOf("\"class\":") + 8;
  if (startPos > 7) {
    String classStr = body.substring(startPos);
    classIdx = classStr.toInt();
  }
  
  if (classIdx >= 0 && classIdx <= 2) {
    Serial.print("   Class: ");
    Serial.println(classIdx);
    
    // Proses klasifikasi
    bool success = sortWaste(classIdx);
    
    if (success) {
      server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Waste sorted\"}");
    } else {
      server.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Bin full\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid class\"}");
  }
}

/*
=============================================================================
HANDLE STATUS - KIRIM STATUS SISTEM
=============================================================================
*/
void handleStatus() {
  String json = "{";
  json += "\"system_ready\":" + String(systemReady ? "true" : "false") + ",";
  json += "\"wifi_connected\":" + String(wifiConnected ? "true" : "false") + ",";
  json += "\"blynk_connected\":" + String(blynkConnected ? "true" : "false") + ",";
  json += "\"organik_count\":" + String(counterOrganik) + ",";
  json += "\"anorganik_count\":" + String(counterAnorganik) + ",";
  json += "\"b3_count\":" + String(counterB3) + ",";
  json += "\"total_processed\":" + String(totalProcessed) + ",";
  json += "\"bin_organik_full\":" + String(binOrganikFull ? "true" : "false") + ",";
  json += "\"bin_anorganik_full\":" + String(binAnorganikFull ? "true" : "false") + ",";
  json += "\"bin_b3_full\":" + String(binB3Full ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

/*
=============================================================================
HANDLE RESET - RESET COUNTER
=============================================================================
*/
void handleReset() {
  counterOrganik = 0;
  counterAnorganik = 0;
  counterB3 = 0;
  totalProcessed = 0;
  
  Serial.println("🔄 Counters RESET!");
  
  server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Counters reset\"}");
}

/*
=============================================================================
HANDLE SERIAL INPUT - TERIMA INPUT DARI SERIAL
=============================================================================
*/
void handleSerialInput() {
  char c = Serial.read();
  
  if (c == '\n' || c == '\r') {
    if (receivedData.length() > 0) {
      int classIdx = receivedData.toInt();
      
      Serial.println("\n📨 Received classification via SERIAL:");
      Serial.print("   Data: ");
      Serial.println(receivedData);
      Serial.print("   Class: ");
      Serial.println(classIdx);
      
      if (classIdx >= 0 && classIdx <= 2) {
        sortWaste(classIdx);
      } else {
        Serial.println("   ❌ Invalid class! Use 0, 1, or 2");
      }
      
      receivedData = "";
    }
  } else {
    receivedData += c;
  }
}

/*
=============================================================================
SORT WASTE - FUNGSI UTAMA PEMILAH SAMPAH
=============================================================================
*/
bool sortWaste(int category) {
  Serial.println("\n========================================");
  Serial.println("🗑️  MEMULAI PROSES PEMILAHAN");
  Serial.println("========================================");
  
  String categoryName;
  int platformPosition;
  
  // Tentukan kategori
  switch (category) {
    case 0:
      categoryName = "ORGANIK";
      platformPosition = SERVO_PLATFORM_ORGANIK;
      break;
    case 1:
      categoryName = "ANORGANIK";
      platformPosition = SERVO_PLATFORM_ANORGANIK;
      break;
    case 2:
      categoryName = "B3";
      platformPosition = SERVO_PLATFORM_B3;
      break;
    default:
      Serial.println("❌ Kategori tidak valid!");
      return false;
  }
  
  Serial.print("📊 Kategori: ");
  Serial.println(categoryName);
  
  // CEK BIN PENUH
  Serial.println("🔍 Checking bin status...");
  bool binFull = checkBinFull(category);
  
  if (binFull) {
    Serial.println("⚠️  BIN PENUH! Tidak dapat memproses.");
    digitalWrite(LED_ERROR, HIGH);
    delay(3000);
    digitalWrite(LED_ERROR, LOW);
    
    // Notifikasi ke Blynk
    if (blynkConnected) {
      Blynk.logEvent("bin_full", "Bin " + categoryName + " sudah PENUH!");
    }
    
    return false;
  }
  
  Serial.println("✅ Bin tersedia");
  
  // STEP 1: PUTAR PLATFORM
  Serial.print("↻  Rotating platform to ");
  Serial.print(categoryName);
  Serial.println("...");
  rotatePlatform(platformPosition);
  delay(SERVO_PLATFORM_DELAY);
  Serial.println("   ✓ Platform in position");
  
  // STEP 2: BUKA PENADAH
  Serial.println("📂 Opening penadah...");
  openPenadah();
  delay(SERVO_PENADAH_DELAY);
  Serial.println("   ✓ Waste dropped");
  
  // STEP 3: TUTUP PENADAH
  Serial.println("📁 Closing penadah...");
  closePenadah();
  Serial.println("   ✓ Penadah closed");
  
  // STEP 4: PLATFORM KE HOME
  Serial.println("⌂  Platform returning to HOME...");
  rotatePlatform(SERVO_PLATFORM_HOME);
  delay(SERVO_PLATFORM_DELAY);
  Serial.println("   ✓ Platform at HOME");
  
  // STEP 5: UPDATE COUNTER
  totalProcessed++;
  switch (category) {
    case 0:
      counterOrganik++;
      break;
    case 1:
      counterAnorganik++;
      break;
    case 2:
      counterB3++;
      break;
  }
  
  Serial.println("\n📈 STATISTIK:");
  Serial.println("   Organik   : " + String(counterOrganik));
  Serial.println("   Anorganik : " + String(counterAnorganik));
  Serial.println("   B3        : " + String(counterB3));
  Serial.println("   TOTAL     : " + String(totalProcessed));
  
  // STEP 6: LED INDICATOR
  setLED(category);
  delay(2000);
  turnOffAllLEDs();
  
  // STEP 7: UPDATE BLYNK
  if (blynkConnected) {
    sendDataToBlynk();
  }
  
  Serial.println("========================================");
  Serial.println("✅ PEMILAHAN SELESAI!");
  Serial.println("========================================\n");
  
  return true;
}

/*
=============================================================================
CHECK BIN FULL - CEK APAKAH BIN PENUH
=============================================================================
*/
bool checkBinFull(int category) {
  float distance;
  int threshold;
  
  switch (category) {
    case 0:  // Organik
      distance = getDistance(TRIG_ORGANIK, ECHO_ORGANIK);
      threshold = THRESHOLD_ORGANIK_FULL;
      binOrganikFull = (distance <= threshold && distance > 0);
      return binOrganikFull;
      
    case 1:  // Anorganik
      distance = getDistance(TRIG_ANORGANIK, ECHO_ANORGANIK);
      threshold = THRESHOLD_ANORGANIK_FULL;
      binAnorganikFull = (distance <= threshold && distance > 0);
      return binAnorganikFull;
      
    case 2:  // B3
      distance = getDistance(TRIG_B3, ECHO_B3);
      threshold = THRESHOLD_B3_FULL;
      binB3Full = (distance <= threshold && distance > 0);
      return binB3Full;
      
    default:
      return false;
  }
}

/*
=============================================================================
CHECK ALL BINS - CEK SEMUA BIN
=============================================================================
*/
void checkAllBins() {
  static unsigned long lastCheck = 0;
  
  // Cek setiap 2 detik (tidak perlu terlalu sering)
  if (millis() - lastCheck < 2000) return;
  
  float distOrganik = getDistance(TRIG_ORGANIK, ECHO_ORGANIK);
  float distAnorganik = getDistance(TRIG_ANORGANIK, ECHO_ANORGANIK);
  float distB3 = getDistance(TRIG_B3, ECHO_B3);
  
  binOrganikFull = (distOrganik <= THRESHOLD_ORGANIK_FULL && distOrganik > 0);
  binAnorganikFull = (distAnorganik <= THRESHOLD_ANORGANIK_FULL && distAnorganik > 0);
  binB3Full = (distB3 <= THRESHOLD_B3_FULL && distB3 > 0);
  
  lastCheck = millis();
}

/*
=============================================================================
GET DISTANCE - BACA JARAK DARI SENSOR ULTRASONIK
=============================================================================
*/
float getDistance(int trigPin, int echoPin) {
  // Kirim trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Baca echo pulse
  long duration = pulseIn(echoPin, HIGH, ULTRASONIC_TIMEOUT);
  
  // Hitung jarak (cm)
  // Speed of sound = 343 m/s = 0.0343 cm/μs
  // Distance = (duration / 2) * 0.0343
  float distance = (duration * 0.0343) / 2.0;
  
  // Return 0 jika timeout atau invalid
  if (duration == 0 || distance > 400) {
    return 0;
  }
  
  return distance;
}

/*
=============================================================================
ROTATE PLATFORM - PUTAR SERVO PLATFORM
=============================================================================
*/
void rotatePlatform(int position) {
  servoPlatform.write(position);
}

/*
=============================================================================
OPEN PENADAH - BUKA SERVO PENADAH
=============================================================================
*/
void openPenadah() {
  servoPenadah.write(SERVO_PENADAH_OPEN);
}

/*
=============================================================================
CLOSE PENADAH - TUTUP SERVO PENADAH
=============================================================================
*/
void closePenadah() {
  servoPenadah.write(SERVO_PENADAH_CLOSE);
}

/*
=============================================================================
SET LED - NYALAKAN LED SESUAI KATEGORI
=============================================================================
*/
void setLED(int category) {
  // Matikan semua LED kategori dulu
  digitalWrite(LED_ORGANIK, LOW);
  digitalWrite(LED_ANORGANIK, LOW);
  digitalWrite(LED_B3, LOW);
  
  // Nyalakan LED sesuai kategori
  switch (category) {
    case 0:
      digitalWrite(LED_ORGANIK, HIGH);
      break;
    case 1:
      digitalWrite(LED_ANORGANIK, HIGH);
      break;
    case 2:
      digitalWrite(LED_B3, HIGH);
      break;
  }
}

/*
=============================================================================
TURN OFF ALL LEDS - MATIKAN SEMUA LED
=============================================================================
*/
void turnOffAllLEDs() {
  digitalWrite(LED_ORGANIK, LOW);
  digitalWrite(LED_ANORGANIK, LOW);
  digitalWrite(LED_B3, LOW);
  digitalWrite(LED_ERROR, LOW);
}

/*
=============================================================================
SEND DATA TO BLYNK - KIRIM DATA KE BLYNK CLOUD
=============================================================================
*/
void sendDataToBlynk() {
  if (!blynkConnected) return;
  
  Blynk.virtualWrite(VPIN_ORGANIK, counterOrganik);
  Blynk.virtualWrite(VPIN_ANORGANIK, counterAnorganik);
  Blynk.virtualWrite(VPIN_B3, counterB3);
  Blynk.virtualWrite(VPIN_TOTAL, totalProcessed);
  
  // Status string
  String status = systemReady ? "READY" : "INITIALIZING";
  Blynk.virtualWrite(VPIN_STATUS, status);
  
  // Bin status (JSON format)
  String binStatus = "{\"organik\":";
  binStatus += binOrganikFull ? "\"FULL\"" : "\"OK\"";
  binStatus += ",\"anorganik\":";
  binStatus += binAnorganikFull ? "\"FULL\"" : "\"OK\"";
  binStatus += ",\"b3\":";
  binStatus += binB3Full ? "\"FULL\"" : "\"OK\"";
  binStatus += "}";
  Blynk.virtualWrite(VPIN_BIN_STATUS, binStatus);
}

/*
=============================================================================
BLYNK RESET BUTTON - HANDLER UNTUK RESET DARI BLYNK
=============================================================================
*/
BLYNK_WRITE(VPIN_RESET) {
  int value = param.asInt();
  
  if (value == 1) {
    counterOrganik = 0;
    counterAnorganik = 0;
    counterB3 = 0;
    totalProcessed = 0;
    
    Serial.println("🔄 Counters RESET via Blynk!");
    
    // Update Blynk
    sendDataToBlynk();
  }
}

/*
=============================================================================
CALIBRATION MODE - MODE KALIBRASI UNTUK TEST SERVO & SENSOR
=============================================================================
Uncomment di setup() untuk mengaktifkan mode kalibrasi.
Mode ini berguna untuk:
1. Test servo penadah (buka/tutup)
2. Test servo platform (rotasi)
3. Test sensor ultrasonik (baca jarak)
4. Cari nilai angle yang tepat untuk servo
=============================================================================
*/
void calibrationMode() {
  Serial.println("\n\n");
  Serial.println("========================================");
  Serial.println("⚙️  MODE KALIBRASI");
  Serial.println("========================================");
  Serial.println("Gunakan mode ini untuk:");
  Serial.println("1. Test servo penadah");
  Serial.println("2. Test servo platform");
  Serial.println("3. Test sensor ultrasonik");
  Serial.println("4. Cari nilai angle yang tepat");
  Serial.println("========================================\n");
  
  Serial.println("📝 MENU:");
  Serial.println("1 = Test Servo Penadah");
  Serial.println("2 = Test Servo Platform");
  Serial.println("3 = Test Sensor Ultrasonik");
  Serial.println("0 = Exit Calibration Mode");
  Serial.println();
  
  bool calibrating = true;
  
  while (calibrating) {
    if (Serial.available()) {
      char cmd = Serial.read();
      
      switch (cmd) {
        case '1':
          testServoPenadah();
          break;
        case '2':
          testServoPlatform();
          break;
        case '3':
          testUltrasonicSensors();
          break;
        case '0':
          calibrating = false;
          Serial.println("\n✅ Exiting calibration mode...\n");
          break;
      }
    }
    delay(10);
  }
}

/*
=============================================================================
TEST SERVO PENADAH - TEST SERVO PENADAH
=============================================================================
*/
void testServoPenadah() {
  Serial.println("\n--- Test Servo Penadah ---");
  Serial.println("Testing CLOSE position...");
  servoPenadah.write(SERVO_PENADAH_CLOSE);
  delay(2000);
  
  Serial.println("Testing OPEN position...");
  servoPenadah.write(SERVO_PENADAH_OPEN);
  delay(2000);
  
  Serial.println("Back to CLOSE...");
  servoPenadah.write(SERVO_PENADAH_CLOSE);
  delay(1000);
  
  Serial.println("✓ Test complete\n");
}

/*
=============================================================================
TEST SERVO PLATFORM - TEST SERVO PLATFORM
=============================================================================
*/
void testServoPlatform() {
  Serial.println("\n--- Test Servo Platform ---");
  
  Serial.println("Moving to ORGANIK position...");
  servoPlatform.write(SERVO_PLATFORM_ORGANIK);
  delay(2000);
  
  Serial.println("Moving to ANORGANIK position...");
  servoPlatform.write(SERVO_PLATFORM_ANORGANIK);
  delay(2000);
  
  Serial.println("Moving to B3 position...");
  servoPlatform.write(SERVO_PLATFORM_B3);
  delay(2000);
  
  Serial.println("Back to HOME...");
  servoPlatform.write(SERVO_PLATFORM_HOME);
  delay(1000);
  
  Serial.println("✓ Test complete\n");
}

/*
=============================================================================
TEST ULTRASONIC SENSORS - TEST SEMUA SENSOR ULTRASONIK
=============================================================================
*/
void testUltrasonicSensors() {
  Serial.println("\n--- Test Ultrasonic Sensors ---");
  Serial.println("Reading distances (10 samples)...\n");
  
  for (int i = 0; i < 10; i++) {
    float distOrganik = getDistance(TRIG_ORGANIK, ECHO_ORGANIK);
    float distAnorganik = getDistance(TRIG_ANORGANIK, ECHO_ANORGANIK);
    float distB3 = getDistance(TRIG_B3, ECHO_B3);
    
    Serial.print("Sample ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print("Organik=");
    Serial.print(distOrganik);
    Serial.print("cm, Anorganik=");
    Serial.print(distAnorganik);
    Serial.print("cm, B3=");
    Serial.print(distB3);
    Serial.println("cm");
    
    delay(500);
  }
  
  Serial.println("\n✓ Test complete\n");
}

/*
=============================================================================
PRINT STATUS - PRINT STATISTIK SISTEM
=============================================================================
*/
void printStatus() {
  Serial.println("\n========================================");
  Serial.println("📊 SYSTEM STATUS");
  Serial.println("========================================");
  Serial.println("WiFi      : " + String(wifiConnected ? "CONNECTED" : "DISCONNECTED"));
  Serial.println("Blynk     : " + String(blynkConnected ? "CONNECTED" : "DISCONNECTED"));
  Serial.println("System    : " + String(systemReady ? "READY" : "NOT READY"));
  Serial.println();
  Serial.println("COUNTERS:");
  Serial.println("  Organik   : " + String(counterOrganik));
  Serial.println("  Anorganik : " + String(counterAnorganik));
  Serial.println("  B3        : " + String(counterB3));
  Serial.println("  TOTAL     : " + String(totalProcessed));
  Serial.println();
  Serial.println("BIN STATUS:");
  Serial.println("  Organik   : " + String(binOrganikFull ? "FULL" : "OK"));
  Serial.println("  Anorganik : " + String(binAnorganikFull ? "FULL" : "OK"));
  Serial.println("  B3        : " + String(binB3Full ? "FULL" : "OK"));
  Serial.println("========================================\n");
}
