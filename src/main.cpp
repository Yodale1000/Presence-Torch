#include <Arduino.h>
#include <WiFiManager.h>
#include <MQTT.h>
#include <WiFi.h>
#include "Ultrasonic.h"
#include <secrets.h>
#include <led.h>

#define THRESHOLD 80 //in cm

#define USS1_PIN 23
#define USS2_PIN 24
#define USS3_PIN 25
#define USS4_PIN 26


Ultrasonic uss_1(USS1_PIN);
Ultrasonic uss_2(USS2_PIN);
Ultrasonic uss_3(USS3_PIN);
Ultrasonic uss_4(USS4_PIN);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

WiFiClient net;
MQTTClient client; 

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
    setAll(255,0,0);
    connect();
  }

  strip.clear();

  int triggeredCount = 0;

  if (uss_1.MeasureInCentimeters() < THRESHOLD) {
    triggeredCount++;
  }
  if (uss_2.MeasureInCentimeters() < THRESHOLD) {
    triggeredCount++;
  }
  if (uss_3.MeasureInCentimeters() < THRESHOLD) {
    triggeredCount++;
  }
  if (uss_4.MeasureInCentimeters() < THRESHOLD) {
    triggeredCount++;
  }

  int brightness = map(0, 0, 4, 0, 255);
  strip.setBrightness(brightness);
  strip.show();
  
  delay(1000);
}