#include <KiwisIoT.h>

/* WiFi + Dashboard */
const char* ssid  = "Your wifi name";
const char* pass  = "password";
const char* topic = " "; // topic ID from dashoard

/* Control Output */
#define RELAY_PIN 14   // GPIO for relay / device

/* KiwisIoT object */
KiwisIoT kiwisiot(ssid, pass, topic);


void onControl(String ch, String val) {

  
  if (ch == "0") {
    if (val == "1") {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println(">>> RELAY ON");
    } else {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println(">>> RELAY OFF");
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);   

  
  kiwisiot.begin();

  
  kiwisiot.onReceive(onControl);
}

void loop() {
  
  kiwisiot.run();
}
