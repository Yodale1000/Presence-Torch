#include <Arduino.h>
#include <WiFiManager.h>
#include <MQTT.h>
#include <WiFi.h>
#include "Ultrasonic.h"
#include <secrets.h>
#include <led.h>

// Konfiguration
#define THRESHOLD 80 // in cm
#define TOGGLE_DISTANCE 1 // in cm
#define DELAY 5000

#define MQTT_TASK_STACK_SIZE 4096
#define SENSOR_TASK_STACK_SIZE 4096

#define NUM_SENSORS 4
#define USS1_PIN 18
#define USS2_PIN 19
#define USS3_PIN 21
#define USS4_PIN 22

Ultrasonic sensors[NUM_SENSORS] = {
  Ultrasonic(USS1_PIN),
  Ultrasonic(USS2_PIN),
  Ultrasonic(USS3_PIN),
  Ultrasonic(USS4_PIN)
};

WiFiClient net;
MQTTClient client; 

unsigned long lastMillis = 0;

// Funktionsdeklarationen
void connectWiFi();
void connectMQTT();
void messageReceived(String &topic, String &payload);
void mqttTask(void *pvParameters);
void sensorTask(void *pvParameters);
void startTasks();

// Verbindung zu WiFi herstellen
void connectWiFi() {
  Serial.print("Checking WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi connected!");
}

// Verbindung zu MQTT herstellen
void connectMQTT() {
  Serial.print("Connecting to MQTT...");
  while (!client.connect(DEVICE_NAME)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nMQTT connected!");
  client.subscribe(MQTT_TOPIC_INCOMING, 2);
}

// Empfangene MQTT-Nachrichten verarbeiten
void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming: " + topic + " - " + payload);
  int triggeredCount = payload.toInt();
  for (int i = 0; i < triggeredCount; i++) {
    FadeInPixel(i, 255, 100, 0);
  }
  delay(DELAY);
  for (int i = triggeredCount; i >= 0; i--) {
    FadeOutPixel(i - 1, 255, 100, 0);
  }
  delay(2000);
}

// Setup-Funktion
void setup() {
  Serial.begin(9600);
  setAll(255, 128, 0);

  WiFiManager wm;
  WiFiManagerParameter custom_device_name("device_name", "device name", DEVICE_NAME, 40);
  wm.addParameter(&custom_device_name);

  WiFiManagerParameter custom_mqtt_server("mqtt_server", "mqtt server", MQTT_SERVER, 40);
  wm.addParameter(&custom_mqtt_server);
  WiFiManagerParameter custom_mqtt_topic_incoming("mqtt_topic_incoming", "mqtt topic incoming", MQTT_TOPIC_INCOMING, 40);
  wm.addParameter(&custom_mqtt_topic_incoming);
  WiFiManagerParameter custom_mqtt_topic_outgoing("mqtt_topic_outgoing", "mqtt topic outgoing", MQTT_TOPIC_OUTGOING, 40);
  wm.addParameter(&custom_mqtt_topic_outgoing);

  bool res = wm.autoConnect(AP_SSID, AP_PASSWORD);

  if (!res) {
    setAll(255, 0, 0); // Rot: Verbindungsfehler
  } else {
    setAll(0, 255, 0); // Grün: Erfolgreich verbunden
  }

  client.begin(custom_mqtt_server.getValue(), net);
  client.onMessage(messageReceived);
  connectWiFi();
  connectMQTT();
  setAll(0, 0, 0);  

  startTasks(); // Starte die FreeRTOS-Tasks
}

// MQTT-Task
void mqttTask(void *pvParameters) {
  while (true) {
    client.loop();
    if (!client.connected()) {
      connectMQTT();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Sensor-Task
void sensorTask(void *pvParameters) {
  while (true) {
    int triggeredCount = 0;

    for (int i = 0; i < 4; i++) {
      int distance = sensors[i].MeasureInCentimeters();
      Serial.print("Sensor "); Serial.print(i+1); Serial.print(": "); Serial.println(distance);

      if (distance < THRESHOLD && distance >= 10) {
        triggeredCount += 4;
      }
    }

    Serial.print("Triggered Count: "); Serial.println(triggeredCount);

    if (triggeredCount > 0 && millis() - lastMillis > 1000) {
      lastMillis = millis();
      client.publish(MQTT_TOPIC_OUTGOING, String(triggeredCount));
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS); // Sensoren jede 2000ms prüfen
  }
}

// FreeRTOS-Tasks starten
void startTasks() {
  xTaskCreate(mqttTask, "MQTT Task", MQTT_TASK_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(sensorTask, "Sensor Task", SENSOR_TASK_STACK_SIZE, NULL, 1, NULL);
}

void loop() {
  // Loop bleibt leer, da alle Aufgaben in FreeRTOS-Tasks ausgelagert wurden
}
