#include "KiwisIoT.h"

/* ===== MQTT SERVER CONFIG ===== */
const char* mqttServer   = "kiwisiot.in";
const int   mqttPort     = 1883;
const char* mqttUser     = "Kiwisiot";
const char* mqttPassword = "Kiwistron@26";

KiwisIoT::KiwisIoT(const char* ssid, const char* pass, const char* topic)
  : client(espClient) {
  _ssid  = ssid;
  _pass  = pass;
  _topic = topic;
}

void KiwisIoT::begin() {
  Serial.println("\n[KiwisIoT] Booting...");
  connectWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback([this](char* t, byte* p, unsigned int l) {
    this->handleMessage(t, p, l);
  });
}

void KiwisIoT::run() {
  if (!client.connected()) connectMQTT();
  client.loop();
}

/* ---------- SEND INT ---------- */
void KiwisIoT::send(const char* channel, int value) {
  String payload = "{\"" + String(channel) + "\":" + String(value) + "}";
  lastTxPayload = payload;
  client.publish(_topic, payload.c_str());
  Serial.println("[TX] " + payload);
}

/* ---------- SEND FLOAT ---------- */
void KiwisIoT::send(const char* channel, float value) {
  String payload = "{\"" + String(channel) + "\":" + String(value, 6) + "}";
  lastTxPayload = payload;
  client.publish(_topic, payload.c_str());
  Serial.println("[TX] " + payload);
}

/* ---------- SEND STRING (GPS, TEXT) ---------- */
void KiwisIoT::send(const char* channel, String value) {
  String payload = "{\"" + String(channel) + "\":\"" + value + "\"}";
  lastTxPayload = payload;
  client.publish(_topic, payload.c_str());
  Serial.println("[TX] " + payload);
}

/* ---------- SEND RAW JSON ---------- */
void KiwisIoT::sendJSON(const char* json) {
  lastTxPayload = String(json);
  client.publish(_topic, json);
  Serial.println("[TX] " + String(json));
}

/* ---------- RECEIVE ---------- */
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

/* ---------- MQTT ---------- */
void KiwisIoT::connectMQTT() {
  Serial.print("[MQTT] Connecting...");

  while (!client.connected()) {
    String clientId = "KiwisIoT-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
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
