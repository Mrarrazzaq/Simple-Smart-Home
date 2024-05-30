#include <WifiEspNowBroadcast.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

static const int LED_PIN = 2;
int ledState = HIGH;
const int MOSFET_PIN = D3; // Ganti dengan pin yang sesuai untuk MOSFET Anda

void processRx(const uint8_t mac[WIFIESPNOW_ALEN], const uint8_t* buf, size_t count, void* arg) {
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (size_t i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();

  if (count > 0) {
    int receivedValue = buf[0] - '0'; // Convert byte to integer value
    if (receivedValue == 3) {
      analogWrite(MOSFET_PIN, 255); // Hidupkan MOSFET (nilai 255 maksimum untuk analogWrite)
    } else if (receivedValue == 4) {
      analogWrite(MOSFET_PIN, 0); // Matikan MOSFET (nilai 0 minimum untuk analogWrite)
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

  pinMode(MOSFET_PIN, OUTPUT);
  analogWrite(MOSFET_PIN, 0); // Pastikan MOSFET mati saat awal

  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());
}

void loop() {
  WifiEspNowBroadcast.loop();
  delay(10);
}
