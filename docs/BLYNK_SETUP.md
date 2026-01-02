# ☁️ Blynk Setup Guide - Sistem Pemilah Sampah Cerdas

Panduan lengkap setup Blynk IoT dashboard untuk monitoring sistem.

---

## 📋 Overview

**Blynk** adalah platform IoT yang memudahkan monitoring dan control device melalui:
- 📱 Mobile app (iOS/Android)
- 💻 Web dashboard
- 🔔 Push notifications
- 📊 Real-time data visualization

**Gratis** untuk hobbyist dengan limit yang cukup untuk project ini.

---

## 🚀 Part 1: Daftar Blynk Account

### Step 1: Sign Up

1. Buka: https://blynk.cloud
2. Klik **"Sign Up"** atau **"Get Started"**
3. Isi form:
   - Email
   - Password
   - Nama
4. Verify email (cek inbox)
5. Login ke dashboard

### Step 2: Pilih Plan

- **Free Plan**: Cukup untuk project ini!
  - 1 organization
  - 10 devices
  - 1 month data retention
  - Basic features

Klik **"Start Free"** atau **"Continue with Free"**

---

## 🏗️ Part 2: Create Template

**Template** adalah blueprint untuk device. Kita buat satu template untuk sistem pemilah sampah.

### Step 1: Create New Template

1. Sidebar → **"Templates"**
2. Klik **"+ New Template"**
3. Isi form:
   - **Name**: `Sistem Pemilah Sampah Cerdas`
   - **Hardware**: `ESP32`
   - **Connection Type**: `WiFi`
   - **Description**: `Smart waste sorting system with YOLOv8 AI`
4. Klik **"Create"**

### Step 2: Setup Datastreams

Datastreams = virtual pins untuk komunikasi.

Klik tab **"Datastreams"** → **"+ New Datastream"**

#### Datastream 1: Counter Organik
- **Type**: Virtual Pin
- **Pin**: `V0`
- **Name**: `Counter Organik`
- **Data Type**: `Integer`
- **Min**: `0`
- **Max**: `1000`
- **Default**: `0`

#### Datastream 2: Counter Anorganik
- **Type**: Virtual Pin
- **Pin**: `V1`
- **Name**: `Counter Anorganik`
- **Data Type**: `Integer`
- **Min**: `0`
- **Max**: `1000`
- **Default**: `0`

#### Datastream 3: Counter B3
- **Type**: Virtual Pin
- **Pin**: `V2`
- **Name**: `Counter B3`
- **Data Type**: `Integer`
- **Min**: `0`
- **Max**: `1000`
- **Default**: `0`

#### Datastream 4: Total Processed
- **Type**: Virtual Pin
- **Pin**: `V3`
- **Name**: `Total Processed`
- **Data Type**: `Integer`
- **Min**: `0`
- **Max**: `5000`
- **Default**: `0`

#### Datastream 5: System Status
- **Type**: Virtual Pin
- **Pin**: `V4`
- **Name**: `System Status`
- **Data Type**: `String`
- **Max Length**: `50`
- **Default**: `INITIALIZING`

#### Datastream 6: Bin Status
- **Type**: Virtual Pin
- **Pin**: `V5`
- **Name**: `Bin Status`
- **Data Type**: `String`
- **Max Length**: `100`
- **Default**: `{"organik":"OK","anorganik":"OK","b3":"OK"}`

#### Datastream 7: Reset Button
- **Type**: Virtual Pin
- **Pin**: `V7`
- **Name**: `Reset Counters`
- **Data Type**: `Integer`
- **Min**: `0`
- **Max**: `1`
- **Default**: `0`

**Save** setelah selesai!

---

## 📊 Part 3: Create Web Dashboard

### Step 1: Open Web Dashboard

1. Template sidebar → **"Web Dashboard"**
2. Klik **"Edit"**
3. Anda akan masuk ke editor mode

### Step 2: Add Widgets

Drag & drop widgets dari sidebar kiri:

#### Widget 1: Value Display - Organik
- Widget: **"Label"** atau **"Value Display"**
- Datastream: `V0 (Counter Organik)`
- Title: `🟢 Organik`
- Font Size: Large
- Color: Green
- Position: Top-left

#### Widget 2: Value Display - Anorganik
- Widget: **"Value Display"**
- Datastream: `V1 (Counter Anorganik)`
- Title: `🔵 Anorganik`
- Font Size: Large
- Color: Blue
- Position: Top-center

#### Widget 3: Value Display - B3
- Widget: **"Value Display"**
- Datastream: `V2 (Counter B3)`
- Title: `🔴 B3`
- Font Size: Large
- Color: Red
- Position: Top-right

#### Widget 4: Value Display - Total
- Widget: **"Value Display"**
- Datastream: `V3 (Total Processed)`
- Title: `📊 Total Sampah Diproses`
- Font Size: X-Large
- Color: Purple
- Position: Center-top

#### Widget 5: System Status
- Widget: **"Label"**
- Datastream: `V4 (System Status)`
- Title: `⚙️ Status Sistem`
- Position: Middle-left

#### Widget 6: Bin Status
- Widget: **"Label"**
- Datastream: `V5 (Bin Status)`
- Title: `🗑️ Status Bin`
- Position: Middle-right

#### Widget 7: Reset Button
- Widget: **"Button"**
- Datastream: `V7 (Reset Counters)`
- Title: `🔄 Reset Counters`
- Mode: `Push` (momentary)
- ON Label: `Reset`
- Position: Bottom

#### Widget 8: Chart (Optional)
- Widget: **"Chart"**
- Datastreams: `V0, V1, V2` (multi-line)
- Title: `📈 Waste Trend`
- Time range: `1 hour` atau `Live`
- Position: Bottom-center

### Step 3: Customize Layout

Drag widgets untuk arrange sesuai preferensi.

**Recommended Layout**:
```
┌─────────────────────────────────────┐
│  🟢 Organik  🔵 Anorganik  🔴 B3   │
│     10           15          5      │
│                                     │
│        📊 Total: 30                 │
│                                     │
│  ⚙️ Status: READY  🗑️ Bin: OK     │
│                                     │
│  📈 [Chart - Trend sampah]         │
│                                     │
│          🔄 Reset Button            │
└─────────────────────────────────────┘
```

Klik **"Save"** setelah selesai.

---

## 📱 Part 4: Create Mobile Dashboard (Optional)

Jika ingin monitoring via smartphone:

### Step 1: Install Blynk App

- iOS: https://apps.apple.com/app/blynk-iot/id1559317868
- Android: https://play.google.com/store/apps/details?id=cloud.blynk

### Step 2: Setup Mobile Dashboard

1. Template → **"Mobile Dashboard"**
2. Klik **"Edit"**
3. Add widgets (sama seperti web dashboard)
4. Arrange sesuai ukuran layar mobile
5. Save

---

## 🔔 Part 5: Setup Events & Notifications

**Events** trigger notifications saat kondisi tertentu terjadi (misal: bin full).

### Step 1: Create Event

1. Template → **"Events"**
2. Klik **"+ New Event"**
3. Isi form:
   - **Event Code**: `bin_full`
   - **Name**: `Bin Full Warning`
   - **Description**: `One or more bins are full`
   - **Color**: Red

### Step 2: Configure Notification

- **Enable Notifications**: ✅ ON
- **Notification Priority**: `High`
- **Notification Message**: 
  ```
  ⚠️ PERINGATAN: Bin sampah sudah penuh! 
  Silakan kosongkan segera.
  ```

### Step 3: Test Notification

Notifications akan di-trigger dari:
- ESP32 (via `Blynk.logEvent("bin_full")`)
- Python script (`blynk_dashboard.py`)

**Save** event.

---

## 🔑 Part 6: Get Auth Token

**Auth Token** dibutuhkan untuk connect device ke Blynk.

### Step 1: Create Device

1. Sidebar → **"Devices"**
2. Klik **"+ New Device"**
3. Pilih **"From Template"**
4. Select: `Sistem Pemilah Sampah Cerdas`
5. Device Name: `ESP32-Main-001`
6. Klik **"Create"**

### Step 2: Copy Auth Token

1. Device akan muncul di list
2. Klik device name
3. Tab **"Device Info"**
4. Copy **"Auth Token"** (panjang ~32 karakter)

**Example**: `a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6`

### Step 3: Update Config Files

#### ESP32 Code (`esp32_main_controller.ino`):
```cpp
char auth[] = "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"; // GANTI!
```

#### Python Config (`config/config.py`):
```python
BLYNK_AUTH = "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"  # GANTI!
```

⚠️ **JANGAN COMMIT AUTH TOKEN KE GITHUB!** (gunakan .env file)

---

## 🧪 Part 7: Testing

### Test 1: ESP32 Connection

1. Upload code ke ESP32 (dengan auth token yang benar)
2. Open Serial Monitor
3. Cek output:
   ```
   ☁️  Connecting to Blynk...
   ✅ Blynk Connected!
   ```
4. Di Blynk dashboard, device status jadi **"Online"** (hijau)

### Test 2: Send Data

Manual test via Serial Monitor:
```
Ketik: 0
```

Expected:
- System proses sampah organik
- Counter Organik di Blynk +1
- Total Processed +1
- Status update

### Test 3: Notification

Simulasi bin full:
```
1. Tutup bin dengan tangan (sensor detect <5cm)
2. Coba proses sampah
3. Expected: Push notification di app/email
```

### Test 4: Reset Button

1. Klik tombol Reset di dashboard
2. Expected: Semua counter jadi 0

---

## 🐍 Part 8: Python Blynk Sync (Optional)

Jika ingin sync otomatis dari laptop, run:

```bash
python inference/blynk_dashboard.py
```

Script ini akan:
- Connect ke Blynk
- Poll ESP32 status tiap 5 detik
- Update dashboard
- Send notifications jika bin full

---

## 📊 Dashboard Customization Tips

### Colors:
- Organik: `#4CAF50` (Green)
- Anorganik: `#2196F3` (Blue)
- B3: `#F44336` (Red)
- Total: `#9C27B0` (Purple)
- Warning: `#FF9800` (Orange)

### Widgets to Add (Advanced):
- **Gauge**: Visual representation counter
- **SuperChart**: Better analytics
- **Terminal**: Debug console
- **Map**: Location tracking (jika mobile)
- **Video Stream**: ESP32-CAM feed

---

## ⚙️ Advanced Settings

### Auto-refresh Interval:
- Default: 1 second
- Recommended: 5 seconds (hemat bandwidth)

### Data Retention:
- Free plan: 1 month
- Paid plan: Up to 1 year

### Organization Settings:
- Template sharing (untuk team)
- Multi-user access
- Custom branding (paid)

---

## 🆘 Troubleshooting

### Device Offline:

**Possible Causes:**
1. ❌ Auth token salah
2. ❌ WiFi tidak connect
3. ❌ Blynk server down
4. ❌ Firewall block

**Solutions:**
1. ✅ Verify auth token
2. ✅ Check WiFi credentials
3. ✅ Ping `blynk.cloud`
4. ✅ Check router firewall

### Data Not Updating:

1. Check device status (online?)
2. Verify datastream configuration
3. Check `Blynk.virtualWrite()` calls in code
4. Monitor Serial output for errors

### Notification Not Received:

1. Check event configuration
2. Enable notifications di app settings
3. Check phone notification permissions
4. Verify `Blynk.logEvent()` called

### Widget Shows "N/A":

- Datastream not configured
- Device never sent data to that pin
- Data type mismatch

---

## 📚 Resources

### Official Docs:
- https://docs.blynk.io - Complete documentation
- https://examples.blynk.cc - Code examples
- https://community.blynk.cc - Community forum

### Video Tutorials:
- YouTube: "Blynk IoT Tutorial"
- YouTube: "ESP32 Blynk Dashboard"

### Library Documentation:
- https://github.com/blynkkk/blynk-library

---

## ✅ Setup Checklist

Sebelum lanjut, pastikan:

- [ ] Blynk account created
- [ ] Template created dengan semua datastreams
- [ ] Web dashboard configured
- [ ] Mobile dashboard configured (optional)
- [ ] Event notification setup
- [ ] Auth token copied
- [ ] Auth token updated di ESP32 code
- [ ] Auth token updated di Python config
- [ ] Device test: online & sending data
- [ ] Dashboard displays correct data
- [ ] Notifications working

---

**Next**: Kembali ke `QUICK_START_GUIDE.md` untuk lanjut testing!
