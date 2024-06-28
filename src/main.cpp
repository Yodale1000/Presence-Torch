#include <Arduino.h>
#include <WiFiManager.h>
#include <MQTT.h>
#include <WiFi.h>
#include "Ultrasonic.h"
#include <secrets.h>
#include <led.h>

#define DELAY 5000
#define THRESHOLD 80

Ultrasonic ultrasonic(23);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiClient net;
MQTTClient client; 

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(DEVICE_NAME)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe(MQTT_TOPIC);
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}


void setup() {
    Serial.begin(115200);
    strip.begin();
    setAll(255,128,0);

    WiFiManager wm;
    bool res;
    res = wm.autoConnect(AP_SSID,AP_PASSWORD);

    if(!res) {
      setAll(255,0,0);
    } 
    else 
    {
      setAll(0,255,0);
    }

    client.begin(MQTT_URL, net);
    client.onMessage(messageReceived);
    connect();
}


void loop() {
  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
  }

/* 
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish(MQTT_TOPIC, "world");
  }
*/
  
  long RangeInCentimeters;
	RangeInCentimeters = ultrasonic.MeasureInCentimeters();

  if (RangeInCentimeters < THRESHOLD)
  {
    FadeIn(255,128,0);
    delay(DELAY);
    FadeOut(255,80,0);
  }
  else
  {
    strip.clear();
    strip.show();
  }
  
  delay(1000);
}