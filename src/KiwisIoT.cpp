#include "KiwisIoT.h"


static const uint8_t _obfKey[] = {
  0x4B, 0x77, 0x21, 0x58, 0x39, 0x7A, 0x51, 0x37,
  0x6D, 0x50, 0x32, 0x76, 0x4C, 0x35, 0x72, 0x54
};
static const size_t _obfKeyLen = sizeof(_obfKey);

static const uint8_t _obfServer[] = {
  0x20, 0x1E, 0x56, 0x31, 0x4A, 0x13, 0x3E, 0x43, 0x43, 0x39, 0x5C
}; 

static const uint8_t _obfUser[] = {
  0x00, 0x1E, 0x56, 0x31, 0x4A, 0x13, 0x3E, 0x43
}; 

static const uint8_t _obfPass[] = {
  0x00, 0x1E, 0x56, 0x31, 0x4A, 0x0E, 0x23, 0x58, 0x03, 0x10, 0x00, 0x40
}; 

static const int mqttPort = 1883;

static String _obfDecode(const uint8_t* data, size_t len) {
  String out;
  out.reserve(len);
  for (size_t i = 0; i < len; i++) {
    out += (char)(data[i] ^ _obfKey[i % _obfKeyLen]);
  }
  return out;
}

KiwisIoT::KiwisIoT(const char* ssid, const char* pass, const char* topic)
  : client(espClient) {
  _ssid  = ssid;
  _pass  = pass;
  _topic = topic;
}

void KiwisIoT::begin() {
  Serial.println("\n[KiwisIoT] Booting...");
  connectWiFi();


  _mqttServer = _obfDecode(_obfServer, sizeof(_obfServer));
  client.setServer(_mqttServer.c_str(), mqttPort);
  client.setCallback([this](char* t, byte* p, unsigned int l) {
    this->handleMessage(t, p, l);
  });
}

void KiwisIoT::run() {
  if (!client.connected()) connectMQTT();
  client.loop();
}


void KiwisIoT::send(const char* channel, int value) {
  String payload = "{\"" + String(channel) + "\":" + String(value) + "}";
  lastTxPayload = payload;
  client.publish(_topic, payload.c_str());
  Serial.println("[TX] " + payload);
}


void KiwisIoT::send(const char* channel, float value) {
  String payload = "{\"" + String(channel) + "\":" + String(value, 6) + "}";
  lastTxPayload = payload;
  client.publish(_topic, payload.c_str());
  Serial.println("[TX] " + payload);
}


void KiwisIoT::send(const char* channel, String value) {
  String payload = "{\"" + String(channel) + "\":\"" + value + "\"}";
  lastTxPayload = payload;
  client.publish(_topic, payload.c_str());
  Serial.println("[TX] " + payload);
}


void KiwisIoT::sendJSON(const char* json) {
  lastTxPayload = String(json);
  client.publish(_topic, json);
  Serial.println("[TX] " + String(json));
}


void KiwisIoT::onReceive(KiwisCallback cb) {
  userCallback = cb;
}

void KiwisIoT::handleMessage(char* topic, byte* payload, unsigned int length) {
  if (String(topic) != _topic) return;

  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  if (msg == lastTxPayload) return;   // echo prevention

  int k1 = msg.indexOf("\"") + 1;
  int k2 = msg.indexOf("\"", k1);
  String ch = msg.substring(k1, k2);

  int v1 = msg.indexOf(":", k2) + 1;
  String val = msg.substring(v1);
  val.replace("}", "");
  val.replace("\"", "");

  Serial.print("[RX] CH=");
  Serial.print(ch);
  Serial.print(" VAL=");
  Serial.println(val);

  if (userCallback) userCallback(ch, val);
}

/* ---------- WIFI ---------- */
void KiwisIoT::connectWiFi() {
  Serial.print("[WiFi] Connecting ");
  WiFi.begin(_ssid, _pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\n[WiFi] Connected IP: " + WiFi.localIP().toString());
}


void KiwisIoT::connectMQTT() {
  Serial.print("[MQTT] Connecting...");

  String user = _obfDecode(_obfUser, sizeof(_obfUser));
  String pass = _obfDecode(_obfPass, sizeof(_obfPass));

  while (!client.connected()) {
    String clientId = "KiwisIoT-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), user.c_str(), pass.c_str())) {
      client.subscribe(_topic);
      Serial.println(" Connected");
      Serial.println("[MQTT] Subscribed: " + String(_topic));
    } else {
      Serial.print(" Failed rc=");
      Serial.println(client.state());
      delay(1500);
    }
  }
}
