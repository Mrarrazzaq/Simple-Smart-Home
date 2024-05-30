#include <WifiEspNowBroadcast.h>
#include <ESP8266WiFi.h> // Library ESP8266WiFi untuk Wemos ESP8266
#include <Servo.h> // Library Servo

static const int LED_PIN = D4; // Pin LED pada Wemos D1 Mini
int ledState = HIGH;

Servo myservo; // Objek servo
int buttonState = 0; // Menyimpan status tombol sebelumnya
int lastButtonState = 0; // Menyimpan status tombol sebelumnya

void processRx(const uint8_t mac[WIFIESPNOW_ALEN], const uint8_t* buf, size_t count, void* arg) {
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (size_t i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();

  if (count > 0) {
    int receivedValue = buf[0] - '0'; // Convert byte to integer value
    if (receivedValue == 5) {
      myservo.write(0); // Set servo ke posisi 0 derajat
    } else if (receivedValue == 6) {
      myservo.write(180); // Set servo ke posisi 180 derajat
    }
  }

  digitalWrite(LED_PIN, ledState);
  ledState = 1 - ledState;
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.persistent(false);
  bool ok = WifiEspNowBroadcast.begin("ESPNOW", 3);
  if (!ok) {
    Serial.println("WifiEspNowBroadcast.begin() failed");
    ESP.restart();
  }

  WifiEspNowBroadcast.onReceive(processRx, nullptr);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);

  pinMode(D1, INPUT_PULLUP); // Mengatur pin D1 sebagai input dengan pull-up resistor internal
  myservo.attach(D3); // Menghubungkan servo ke pin D3 pada Wemos D1 Mini
  myservo.write(0); // Pastikan servo berada pada posisi awal (0 derajat)

  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());
}

void loop() {
  WifiEspNowBroadcast.loop();

  // Membaca status tombol
  buttonState = digitalRead(D1);

  // Memeriksa jika tombol ditekan dan status sebelumnya tidak menyala
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Mengubah posisi servo
    if (myservo.read() == 0) {
      myservo.write(180); // Jika servo pada 0 derajat, pindahkan ke 180 derajat
    } else {
      myservo.write(0); // Jika servo pada 180 derajat, pindahkan ke 0 derajat
    }
  }

  // Menyimpan status tombol sebelumnya
  lastButtonState = buttonState;

  delay(10);
}
