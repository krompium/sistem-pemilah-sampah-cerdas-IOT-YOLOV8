"""
=============================================================================
SISTEM PEMILAH SAMPAH CERDAS - BLYNK DASHBOARD INTEGRATION
Script untuk update Blynk IoT dashboard
=============================================================================

FITUR:
✅ Send data ke Blynk virtual pins
✅ Push notifications (bin full warning)
✅ Real-time dashboard update
✅ Manual control (reset counters)
✅ Image upload (last captured waste)

VIRTUAL PINS:
V0 = Counter Organik
V1 = Counter Anorganik
V2 = Counter B3
V3 = Total Processed
V4 = System Status
V5 = Bin Full Status (JSON)
V6 = Last Image
V7 = Reset Button

CARA PAKAI:
1. Daftar di https://blynk.cloud
2. Create new template
3. Setup virtual pins sesuai di atas
4. Copy Auth Token ke config.py
5. Run script: python blynk_dashboard.py

Author: @krompium
Untuk: UAS Sistem Pemilah Sampah Cerdas
=============================================================================
"""

import os
import sys
import time
import requests
from datetime import datetime
import threading

# Import config
try:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'config'))
    from config import *
except ImportError:
    print("⚠️  Warning: config.py tidak ditemukan")
    # Default config
    BLYNK_AUTH = "YourBlynkAuthToken"
    BLYNK_SERVER = "blynk.cloud"
    BLYNK_PORT = 80
    ESP32_MAIN_IP = "192.168.1.102"
    ESP32_MAIN_PORT = 80

# ========== BLYNK API ==========
class BlynkDashboard:
    """
    Class untuk handle Blynk communication
    """
    
    def __init__(self, auth_token, server="blynk.cloud", port=80):
        self.auth_token = auth_token
        self.server = server
        self.port = port
        self.base_url = f"http://{server}:{port}/{auth_token}"
        self.connected = False
        
        print("☁️  Initializing Blynk Dashboard...")
        print(f"   Server: {server}")
        
        # Test connection
        if self.test_connection():
            self.connected = True
            print("   ✅ Blynk connected!")
        else:
            self.connected = False
            print("   ❌ Blynk connection failed!")
    
    def test_connection(self):
        """
        Test connection ke Blynk server
        """
        try:
            url = f"{self.base_url}/isHardwareConnected"
            response = requests.get(url, timeout=5)
            return response.status_code == 200
        except Exception as e:
            print(f"   Error: {e}")
            return False
    
    def virtual_write(self, pin, value):
        """
        Write value ke virtual pin
        """
        if not self.connected:
            return False
        
        try:
            url = f"{self.base_url}/update/{pin}"
            params = {'value': value}
            response = requests.get(url, params=params, timeout=5)
            return response.status_code == 200
        except Exception as e:
            print(f"❌ Error writing to {pin}: {e}")
            return False
    
    def virtual_write_bulk(self, pin_values):
        """
        Write multiple pins sekaligus (lebih efisien)
        """
        if not self.connected:
            return False
        
        try:
            url = f"{self.base_url}/batch/update"
            params = pin_values
            response = requests.get(url, params=params, timeout=5)
            return response.status_code == 200
        except Exception as e:
            print(f"❌ Error bulk write: {e}")
            return False
    
    def log_event(self, event_name, description=""):
        """
        Log event (untuk notifications)
        """
        if not self.connected:
            return False
        
        try:
            url = f"{self.base_url}/logEvent/{event_name}"
            if description:
                params = {'description': description}
                response = requests.get(url, params=params, timeout=5)
            else:
                response = requests.get(url, timeout=5)
            return response.status_code == 200
        except Exception as e:
            print(f"❌ Error logging event: {e}")
            return False
    
    def send_notification(self, message):
        """
        Send push notification
        """
        if not self.connected:
            return False
        
        try:
            url = f"{self.base_url}/notify"
            params = {'body': message}
            response = requests.get(url, params=params, timeout=5)
            return response.status_code == 200
        except Exception as e:
            print(f"❌ Error sending notification: {e}")
            return False

# ========== DATA SYNC ==========
class DataSync:
    """
    Class untuk sync data antara ESP32 dan Blynk
    """
    
    def __init__(self, blynk, esp32_ip, esp32_port):
        self.blynk = blynk
        self.esp32_ip = esp32_ip
        self.esp32_port = esp32_port
        self.last_sync = 0
        self.sync_interval = 5  # seconds
        
        # Cache untuk menghindari duplicate notifications
        self.last_bin_status = {
            'organik': False,
            'anorganik': False,
            'b3': False
        }
    
    def get_esp32_status(self):
        """
        Get status dari ESP32 Main Controller
        """
        try:
            url = f"http://{self.esp32_ip}:{self.esp32_port}/status"
            response = requests.get(url, timeout=5)
            
            if response.status_code == 200:
                return response.json()
            else:
                return None
        except Exception as e:
            print(f"⚠️  Error getting ESP32 status: {e}")
            return None
    
    def sync_to_blynk(self):
        """
        Sync data dari ESP32 ke Blynk
        """
        # Get data dari ESP32
        status = self.get_esp32_status()
        
        if status is None:
            print("⚠️  Cannot sync: ESP32 not responding")
            return False
        
        print("\n📊 Syncing to Blynk...")
        
        # Update counters (bulk write untuk efisiensi)
        pin_values = {
            'V0': status.get('organik_count', 0),
            'V1': status.get('anorganik_count', 0),
            'V2': status.get('b3_count', 0),
            'V3': status.get('total_processed', 0)
        }
        
        success = self.blynk.virtual_write_bulk(pin_values)
        
        if success:
            print("   ✓ Counters updated")
        
        # Update system status
        system_status = "READY" if status.get('system_ready', False) else "NOT READY"
        self.blynk.virtual_write('V4', system_status)
        
        # Update bin status
        bin_status = {
            'organik': status.get('bin_organik_full', False),
            'anorganik': status.get('bin_anorganik_full', False),
            'b3': status.get('bin_b3_full', False)
        }
        
        # Format JSON untuk V5
        bin_status_json = str(bin_status).replace("'", '"')
        self.blynk.virtual_write('V5', bin_status_json)
        
        # Check bin full dan kirim notification jika perlu
        self.check_bin_full_notifications(bin_status)
        
        print("   ✓ Sync complete")
        
        return True
    
    def check_bin_full_notifications(self, bin_status):
        """
        Check bin full status dan kirim notification
        """
        for bin_name, is_full in bin_status.items():
            # Jika bin baru penuh (tidak penuh sebelumnya)
            if is_full and not self.last_bin_status[bin_name]:
                message = f"⚠️ BIN {bin_name.upper()} SUDAH PENUH! Silakan kosongkan."
                
                # Send notification
                self.blynk.send_notification(message)
                
                # Log event
                self.blynk.log_event('bin_full', f"Bin {bin_name} full")
                
                print(f"   🔔 Notification sent: {message}")
            
            # Update cache
            self.last_bin_status[bin_name] = is_full
    
    def run_continuous_sync(self):
        """
        Run continuous sync di background thread
        """
        print("\n🔄 Starting continuous sync...")
        print(f"   Interval: {self.sync_interval} seconds")
        
        while True:
            try:
                self.sync_to_blynk()
                time.sleep(self.sync_interval)
            except KeyboardInterrupt:
                print("\n🛑 Sync stopped by user")
                break
            except Exception as e:
                print(f"❌ Sync error: {e}")
                time.sleep(self.sync_interval)

# ========== MAIN ==========
def main():
    """
    Main function
    """
    print("\n" + "=" * 70)
    print("☁️  SISTEM PEMILAH SAMPAH CERDAS - BLYNK DASHBOARD")
    print("=" * 70)
    print()
    
    # Check config
    if BLYNK_AUTH == "YourBlynkAuthToken":
        print("❌ Error: Blynk Auth Token belum di-set!")
        print("Silakan update BLYNK_AUTH di config.py")
        sys.exit(1)
    
    # Initialize Blynk
    blynk = BlynkDashboard(BLYNK_AUTH, BLYNK_SERVER, BLYNK_PORT)
    
    if not blynk.connected:
        print("❌ Cannot connect to Blynk")
        print("Periksa:")
        print("1. Auth Token sudah benar?")
        print("2. Internet connection OK?")
        print("3. Blynk template sudah dibuat?")
        sys.exit(1)
    
    # Initialize data sync
    sync = DataSync(blynk, ESP32_MAIN_IP, ESP32_MAIN_PORT)
    
    print("\n✅ SISTEM SIAP!")
    print("=" * 70)
    print("Dashboard akan update otomatis setiap 5 detik")
    print("Tekan Ctrl+C untuk stop")
    print("=" * 70)
    print()
    
    # Run continuous sync
    try:
        sync.run_continuous_sync()
    except KeyboardInterrupt:
        print("\n\n✅ Dashboard sync stopped")

# ========== STANDALONE FUNCTIONS ==========
def send_test_data():
    """
    Send test data ke Blynk (untuk testing)
    """
    print("🧪 Sending test data to Blynk...")
    
    blynk = BlynkDashboard(BLYNK_AUTH, BLYNK_SERVER, BLYNK_PORT)
    
    if not blynk.connected:
        print("❌ Cannot connect to Blynk")
        return
    
    # Test counters
    blynk.virtual_write('V0', 10)  # Organik
    blynk.virtual_write('V1', 15)  # Anorganik
    blynk.virtual_write('V2', 5)   # B3
    blynk.virtual_write('V3', 30)  # Total
    
    # Test status
    blynk.virtual_write('V4', 'TESTING')
    
    # Test notification
    blynk.send_notification('Test notification dari sistem pemilah sampah!')
    
    print("✅ Test data sent!")

if __name__ == '__main__':
    # Uncomment untuk test
    # send_test_data()
    
    # Run main sync
    main()
