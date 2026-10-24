#ifndef KIWISIOT_H
#define KIWISIOT_H

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#else
  #error "ESP8266 / ESP32 only"
#endif

#include <PubSubClient.h>

typedef void (*KiwisCallback)(String channel, String value);

class KiwisIoT {
  public:
    KiwisIoT(const char* ssid, const char* pass, const char* topic);

    void begin();
    void run();

    
    void send(const char* channel, int value);       
    void send(const char* channel, float value);      
    void send(const char* channel, String value);     
    void sendJSON(const char* json);                  

    void onReceive(KiwisCallback cb);

  private:
    const char* _ssid;
    const char* _pass;
    const char* _topic;

    WiFiClient espClient;
    PubSubClient client;
    KiwisCallback userCallback;

    String lastTxPayload;   
    String _mqttServer;     

    void connectWiFi();
    void connectMQTT();
    void handleMessage(char* topic, byte* payload, unsigned int length);
};

#endif
