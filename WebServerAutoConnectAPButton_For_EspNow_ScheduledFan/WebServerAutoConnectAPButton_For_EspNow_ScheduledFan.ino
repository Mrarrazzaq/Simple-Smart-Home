#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <RCSwitch.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

ESP8266WebServer server(80);
RCSwitch mySwitch = RCSwitch();
int transmitPin = 0; // Pin GPIO used for RF transmission, in this example, pin D3.
const int ledPin = 2; // LED built-in on the ESP8266
const int buttonPin = 4; // Pin GPIO for the pushbutton, change it to your selected pin
bool inAPMode = false;

unsigned long lastWifiCheckTime = 0;
const unsigned long wifiCheckInterval = 60000; // Pengecekan WiFi setiap 1 menit
unsigned long startTime = 0;
const unsigned long resetInterval = 6 * 60 * 60 * 1000; // Reset setiap 6 jam
const int timeZoneOffset = 7 * 3600; // UTC+7 in seconds

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  if (digitalRead(buttonPin) == LOW) {
    // Button is pressed, enter AP mode
    inAPMode = true;
  } else {
    // Inisialisasi WiFiManager
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(10);

    // Cek koneksi WiFi yang telah dikonfigurasi sebelumnya
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Tidak terhubung ke WiFi yang dikonfigurasi. Membuka konfigurasi WiFiManager...");
      if (!wifiManager.autoConnect("ESP8266-AP")) {
        Serial.println("Gagal terhubung ke WiFiManager. Reset untuk mencoba lagi.");
        delay(500);
        ESP.reset();
        delay(500);
      }
    } else {
      // Jika berhasil terhubung ke WiFi yang telah dikonfigurasi
      Serial.println("Terhubung ke WiFi.");
      digitalWrite(ledPin, HIGH); // Nyalakan LED bawaan
    }
  }

  if (inAPMode) {
    // Masuk ke mode AP dan aktifkan WiFiManager tanpa kata sandi
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(180); // Waktu dalam detik
    wifiManager.startConfigPortal("ESP8266-AP"); // Tanpa kata sandi
  }

  // Inisialisasi RCSwitch
  mySwitch.enableTransmit(transmitPin);

  // Inisialisasi NTPClient
  timeClient.begin();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/button1", HTTP_GET, handleButton1);
  server.on("/button2", HTTP_GET, handleButton2);
  server.on("/button3", HTTP_GET, handleButton3);
  server.on("/button4", HTTP_GET, handleButton4);
  server.on("/button5", HTTP_GET, handleButton5);
  server.on("/button6", HTTP_GET, handleButton6);

  server.begin();
  
  startTime = millis(); // Mulai waktu saat ESP dinyalakan
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    // Button is pressed, enter AP mode
    inAPMode = true;
  }
  
  // Handle permintaan klien
  server.handleClient();

  unsigned long currentMillis = millis();
  if (currentMillis - lastWifiCheckTime >= wifiCheckInterval) {
    // Waktu untuk memeriksa status WiFi
    lastWifiCheckTime = currentMillis;
    if (WiFi.status() != WL_CONNECTED) {
      // Tidak terhubung ke WiFi, lakukan reset
      Serial.println("Tidak terhubung ke WiFi. Melakukan reset...");
      delay(500);
      ESP.reset();
      delay(500);
    }
  }
  if (millis() - startTime >= resetInterval) {
    Serial.println("Waktu reset telah tiba. Melakukan reset ESP...");
    delay(1000); // Berikan sedikit waktu sebelum reset
    digitalWrite(ledPin, LOW);
    ESP.reset();
  }

  // Mendapatkan waktu saat ini dari server NTP
  timeClient.update();
  int currentHour = (timeClient.getHours() + timeZoneOffset / 3600) % 24;
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  // Logic to write to Serial Monitor
  if (currentHour == 6 && currentMinute == 1 && currentSecond == 1) {
    Serial.println("3");
    delay(5000);
    Serial.println("3");
    delay(5000);
    Serial.println("3");
    delay(5000);
  } else if (currentHour == 18 && currentMinute == 1 && currentSecond == 1) {
    Serial.println("4");
    delay(5000);
    Serial.println("4");
    delay(5000);
    Serial.println("4");
    delay(5000);
  }
}

void handleRoot() {
  // Kirim halaman HTML dengan 6 tombol
  String html = "<html><body>";
  html += "<h1>ESP8266 Web Server</h1>";
  for (int i = 1; i <= 6; i++) {
    html += "<button><a href='/button" + String(i) + "'>Button " + String(i) + "</a></button><br>";
  }
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void sendRFSignal(unsigned long code, unsigned int length, unsigned long delayTime) {
  mySwitch.send(code, length);
  delay(delayTime);
}

void handleButton1() {
  // Tanggapan ketika tombol 1 ditekan
  server.send(200, "text/plain", "Button 1 pressed");
  sendRFSignal(13313697, 24, 1000); // Mengirim sinyal RF selama 1 detik
  Serial.println("Button 1 pressed");
}

void handleButton2() {
  // Tanggapan ketika tombol 2 ditekan
  server.send(200, "text/plain", "Button 2 pressed");
  sendRFSignal(13313698, 24, 1000); // Mengirim sinyal RF selama 1 detik
  Serial.println("Button 2 pressed");
}

void handleButton3() {
  // Tanggapan ketika tombol 3 ditekan
  server.send(200, "text/plain", "Button 3 pressed");
  Serial.println("3");
}

void handleButton4() {
  // Tanggapan ketika tombol 4 ditekan
  server.send(200, "text/plain", "Button 4 pressed");
  Serial.println("4");
}

void handleButton5() {
  // Tanggapan ketika tombol 5 ditekan
  server.send(200, "text/plain", "Button 5 pressed");
  Serial.println("5");
}

void handleButton6() {
  // Tanggapan ketika tombol 6 ditekan
  server.send(200, "text/plain", "Button 6 pressed");
  Serial.println("6");
}
