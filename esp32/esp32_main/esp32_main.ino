/*
 * SISTEM PEMILAH SAMPAH CERDAS - ESP32 MAIN CONTROLLER
 * =====================================================
 * 
 * Hardware:
 * - ESP32 DevKit V1
 * - 3x Servo SG90 (Organik, Anorganik, B3)
 * - Sensor Ultrasonik HC-SR04
 * - 3x LED Indicator
 * 
 * Fungsi:
 * - Deteksi objek dengan sensor ultrasonik
 * - Kontrol servo untuk pemilahan sampah
 * - Terima perintah klasifikasi dari laptop via Serial
 * - Kirim data ke Blynk
 * - Indikator LED untuk setiap kategori
 */

#include <ESP32Servo.h>

// ========== PIN CONFIGURATION ==========
// Servo Motors
#define SERVO_ORGANIK_PIN    12
#define SERVO_ANORGANIK_PIN  13
#define SERVO_B3_PIN         14

// Ultrasonic Sensor
#define TRIG_PIN  26
#define ECHO_PIN  27

// LED Indicators
#define LED_ORGANIK    32
#define LED_ANORGANIK  33
#define LED_B3         25

// ========== SERVO OBJECTS ==========
Servo servoOrganik;
Servo servoAnorganik;
Servo servoB3;

// ========== CONSTANTS ==========
const int SERVO_OPEN = 90;    // Posisi servo saat membuka
const int SERVO_CLOSE = 0;    // Posisi servo saat menutup
const int SERVO_DELAY = 2000; // Delay (ms) sebelum menutup kembali
const int DISTANCE_THRESHOLD = 15; // Jarak deteksi (cm)

// ========== VARIABLES ==========
int countOrganik = 0;
int countAnorganik = 0;
int countB3 = 0;

// ========== SETUP ==========
void setup() {
  // Serial untuk komunikasi dengan laptop
  Serial.begin(115200);
  Serial.println("========================================");
  Serial.println("SISTEM PEMILAH SAMPAH CERDAS");
  Serial.println("ESP32 Main Controller");
  Serial.println("========================================");
  
  // Setup Servo
  servoOrganik.attach(SERVO_ORGANIK_PIN);
  servoAnorganik.attach(SERVO_ANORGANIK_PIN);
  servoB3.attach(SERVO_B3_PIN);
  
  // Set posisi awal servo (tertutup)
  servoOrganik.write(SERVO_CLOSE);
  servoAnorganik.write(SERVO_CLOSE);
  servoB3.write(SERVO_CLOSE);
  
  // Setup Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Setup LED
  pinMode(LED_ORGANIK, OUTPUT);
  pinMode(LED_ANORGANIK, OUTPUT);
  pinMode(LED_B3, OUTPUT);
  
  // Test LED (blink 3x)
  testLED();
  
  Serial.println("\n✅ Setup selesai!");
  Serial.println("⏳ Menunggu sampah...\n");
}

// ========== MAIN LOOP ==========
void loop() {
  // Cek jarak dengan sensor ultrasonik
  float distance = getDistance();
  
  // Jika ada objek terdeteksi
  if (distance > 0 && distance < DISTANCE_THRESHOLD) {
    Serial.println("🗑️ Sampah terdeteksi!");
    Serial.print("   Jarak: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // Kirim sinyal ke laptop untuk klasifikasi
    Serial.println("CAM:CAPTURE");
    
    // Tunggu hasil klasifikasi dari laptop
    waitForClassification();
    
    delay(1000); // Delay sebelum deteksi berikutnya
  }
  
  // Cek jika ada perintah dari Serial (laptop)
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    processCommand(command);
  }
  
  delay(100);
}

// ========== FUNGSI: GET DISTANCE ==========
float getDistance() {
  // Kirim trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Baca echo
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout 30ms
  
  // Hitung jarak (cm)
  if (duration == 0) return -1; // Timeout
  float distance = duration * 0.034 / 2;
  
  return distance;
}

// ========== FUNGSI: WAIT FOR CLASSIFICATION ==========
void waitForClassification() {
  Serial.println("⏳ Menunggu hasil klasifikasi...");
  
  unsigned long startTime = millis();
  unsigned long timeout = 10000; // Timeout 10 detik
  
  while (millis() - startTime < timeout) {
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      command.trim();
      
      if (command.startsWith("CLASS:")) {
        processCommand(command);
        return;
      }
    }
    delay(100);
  }
  
  Serial.println("⚠️ Timeout: Tidak menerima hasil klasifikasi");
}

// ========== FUNGSI: PROCESS COMMAND ==========
void processCommand(String command) {
  Serial.print("📥 Perintah diterima: ");
  Serial.println(command);
  
  if (command == "CLASS:0" || command == "0") {
    sortWaste(0); // Organik
  }
  else if (command == "CLASS:1" || command == "1") {
    sortWaste(1); // Anorganik
  }
  else if (command == "CLASS:2" || command == "2") {
    sortWaste(2); // B3
  }
  else if (command == "STATUS") {
    printStatus();
  }
  else if (command == "RESET") {
    resetCounters();
  }
  else {
    Serial.println("❌ Perintah tidak dikenali!");
  }
}

// ========== FUNGSI: SORT WASTE ==========
void sortWaste(int category) {
  String categoryName;
  Servo* servo;
  int led;
  
  // Tentukan kategori
  switch(category) {
    case 0:
      categoryName = "ORGANIK";
      servo = &servoOrganik;
      led = LED_ORGANIK;
      countOrganik++;
      break;
    case 1:
      categoryName = "ANORGANIK";
      servo = &servoAnorganik;
      led = LED_ANORGANIK;
      countAnorganik++;
      break;
    case 2:
      categoryName = "B3";
      servo = &servoB3;
      led = LED_B3;
      countB3++;
      break;
    default:
      Serial.println("❌ Kategori tidak valid!");
      return;
  }
  
  // Tampilkan info
  Serial.println("========================================");
  Serial.print("📤 MEMILAH: ");
  Serial.println(categoryName);
  Serial.println("========================================");
  
  // Nyalakan LED
  digitalWrite(led, HIGH);
  
  // Buka servo
  servo->write(SERVO_OPEN);
  Serial.println("   ➜ Servo membuka...");
  
  delay(SERVO_DELAY);
  
  // Tutup servo
  servo->write(SERVO_CLOSE);
  Serial.println("   ➜ Servo menutup...");
  
  // Matikan LED
  digitalWrite(led, LOW);
  
  Serial.println("✅ Pemilahan selesai!\n");
  
  // Print statistik
  printStatus();
}

// ========== FUNGSI: PRINT STATUS ==========
void printStatus() {
  Serial.println("\n📊 STATISTIK PEMILAHAN:");
  Serial.print("   Organik   : ");
  Serial.println(countOrganik);
  Serial.print("   Anorganik : ");
  Serial.println(countAnorganik);
  Serial.print("   B3        : ");
  Serial.println(countB3);
  Serial.print("   Total     : ");
  Serial.println(countOrganik + countAnorganik + countB3);
  Serial.println();
}

// ========== FUNGSI: RESET COUNTERS ==========
void resetCounters() {
  countOrganik = 0;
  countAnorganik = 0;
  countB3 = 0;
  Serial.println("🔄 Counter di-reset!");
  printStatus();
}

// ========== FUNGSI: TEST LED ==========
void testLED() {
  Serial.println("🔦 Testing LED...");
  
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_ORGANIK, HIGH);
    digitalWrite(LED_ANORGANIK, HIGH);
    digitalWrite(LED_B3, HIGH);
    delay(200);
    
    digitalWrite(LED_ORGANIK, LOW);
    digitalWrite(LED_ANORGANIK, LOW);
    digitalWrite(LED_B3, LOW);
    delay(200);
  }
  
  Serial.println("✅ LED test selesai!");
}
