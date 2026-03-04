#include <KiwisIoT.h>

const char* ssid  = "Your Wifi name";
const char* pass  = "password";
const char* topic = "  "; // topic ID from dashboard

#define SENSOR_PIN 34   // Analog sensor pin (ESP32)

KiwisIoT kiwisiot(ssid, pass, topic);

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  kiwisiot.begin();
}

void loop() {
  kiwisiot.run();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 2000) {
    lastSend = millis();

    int sensorValue = analogRead(SENSOR_PIN);
    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);

    kiwisiot.send("2", sensorValue);  // 2 is channel number
  }
}
