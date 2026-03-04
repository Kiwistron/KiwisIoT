#include <KiwisIoT.h>

/* WiFi + Dashboard */
const char* ssid  = "your wifi name";
const char* pass  = "password";
const char* topic = " ";// tpoic id from dashboard

/* KiwisIoT object */
KiwisIoT kiwisiot(ssid, pass, topic);

void onControl(String ch, String val) {

  Serial.println("===== DATA RECEIVED =====");
  Serial.print("Channel: ");
  Serial.println(ch);
  Serial.print("Value: ");
  Serial.println(val);
  Serial.println("=========================");
}

void setup() {
  Serial.begin(115200);

  kiwisiot.begin();
  kiwisiot.onReceive(onControl);
}

void loop() {
  kiwisiot.run();
}
