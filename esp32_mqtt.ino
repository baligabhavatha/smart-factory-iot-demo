#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ── WiFi ─────────────────────────
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ── MQTT ─────────────────────────
const char* mqttServer = "broker.hivemq.com";
const int   mqttPort   = 1883;
const char* mqttTopic  = "factory/machine01/data";
const char* deviceID   = "MACHINE-01";

// ── DHT22 ────────────────────────
#define DHT_PIN  4
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// ── MPU6050 ─────────────────────
Adafruit_MPU6050 mpu;

// ── Clients ─────────────────────
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// ── WiFi Connect ─────────────────
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

// ── MQTT Connect ────────────────
void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqtt.connect(deviceID)) {
      Serial.println(" Connected!");
    } else {
      Serial.println(" Failed. Retrying...");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Init DHT
  dht.begin();

  // Init MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }
  Serial.println("MPU6050 Initialized");

  // WiFi + MQTT
  connectWiFi();
  mqtt.setServer(mqttServer, mqttPort);
  connectMQTT();
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  // ── Read DHT ───────────────
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  // ── Read MPU6050 ───────────
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  // ── Print ──────────────────
  Serial.printf("Temp: %.1f°C | Hum: %.1f%%\n", temp, hum);
  Serial.printf("Accel: X=%.2f Y=%.2f Z=%.2f m/s²\n", a.acceleration.x, a.acceleration.y, a.acceleration.z);
  Serial.printf("Gyro : X=%.2f Y=%.2f Z=%.2f rad/s\n", g.gyro.x, g.gyro.y, g.gyro.z);

  // ── JSON Payload ───────────
  StaticJsonDocument<256> doc;

  doc["device"] = deviceID;

  doc["temp"] = temp;
  doc["hum"]  = hum;

  doc["accel_x"] = a.acceleration.x;
  doc["accel_y"] = a.acceleration.y;
  doc["accel_z"] = a.acceleration.z;

  doc["gyro_x"] = g.gyro.x;
  doc["gyro_y"] = g.gyro.y;
  doc["gyro_z"] = g.gyro.z;

  char payload[256];
  serializeJson(doc, payload);

  // ── Publish ────────────────
  mqtt.publish(mqttTopic, payload);
  Serial.println("Published: " + String(payload));

  delay(2000);
}
