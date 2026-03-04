#include <KiwisIoT.h>
#include <TinyGPSPlus.h>

const char* ssid  = "Your wifi name";
const char* pass  = "password";
const char* topic = " "; // topic id from dashboard


TinyGPSPlus gps;
HardwareSerial gpsSerial(2);   // UART2

KiwisIoT kiwisiot(ssid, pass, topic);


#define MAP_CH "0"   

void setup() {
  Serial.begin(115200);

  // GPS Serial → RX=16 TX=17
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  kiwisiot.begin();
}

void loop() {
  kiwisiot.run();

  // Read GPS bytes
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 3000) {
    lastSend = millis();

    if (gps.location.isValid()) {
      float lat = gps.location.lat();
      float lng = gps.location.lng();

      Serial.print("LAT: ");
      Serial.println(lat);
      Serial.print("LNG: ");
      Serial.println(lng);

      
      String loc = String(lat, 6) + "," + String(lng, 6);

      // Send to SINGLE map channel
      kiwisiot.send(MAP_CH, loc);
    }
  }
}
