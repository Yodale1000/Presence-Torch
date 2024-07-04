#include <Arduino.h>
#include <WiFiManager.h>
#include <MQTT.h>
#include <WiFi.h>
#include "Ultrasonic.h"
#include <secrets.h>
#include <led.h>

#define THRESHOLD 80 //in cm
#define TOGGLE_DISTANCE 10 //in cm

#define USS1_PIN 22
#define USS2_PIN 1
#define USS3_PIN 3
#define USS4_PIN 21


Ultrasonic uss_1(USS1_PIN);
Ultrasonic uss_2(USS2_PIN);
Ultrasonic uss_3(USS3_PIN);
Ultrasonic uss_4(USS4_PIN);

bool activeSensors[4] = {true, true, true, true};

int currentBrightness = 0;
int fadeDelay = 10;  

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
    setAll(255,128,0);

    WiFiManager wm;

    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", MQTT_URL, 40);
    wm.addParameter(&custom_mqtt_server);
    WiFiManagerParameter custom_mqtt_topic("topic", "mqtt topic", MQTT_TOPIC, 40);
    wm.addParameter(&custom_mqtt_topic);



    bool res;
    res = wm.autoConnect(AP_SSID,AP_PASSWORD);

    if(!res) {
      setAll(255,0,0);
    } 
    else 
    {
      setAll(0,255,0);
    }

    client.begin(custom_mqtt_server.getValue(), net);
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

  Serial.println("start:");

  int triggeredCount = 0;
  if (uss_1.MeasureInCentimeters() < THRESHOLD) {
    triggeredCount++;
  }
  if (uss_3.MeasureInCentimeters() < THRESHOLD) {
    triggeredCount++;
  }
/*  if (uss_2.MeasureInCentimeters() < THRESHOLD) {
    triggeredCount++;
  }
  if (uss_4.MeasureInCentimeters() < THRESHOLD) {
    triggeredCount++;
  }*/

  Serial.println(triggeredCount);

  int brightness = map(triggeredCount, 0, 4, 0, 255);
  Serial.println(brightness);
  SetBrightness(brightness);
  FadeIn(255,0,0);
  delay(2000);
  FadeOut(255,0,0);
}