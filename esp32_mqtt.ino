#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ── WiFi ─────────────────────────
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ── MQTT ─────────────────────────
const char* mqttServer = "broker.hivemq.com";
const int   mqttPort   = 1883;
const char* mqttTopic  = "factory/machine01/dht";
const char* cmdTopic   = "factory/machine01/cmd";   // ← listens here
const char* deviceID   = "MACHINE-01";

// ── DHT22 ────────────────────────
#define DHT_PIN  4
#define DHT_TYPE DHT22
#define LED_PIN  26    // ← matches your wokwi wiring

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// ── MQTT Callback ────────────────
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();

  Serial.println("CMD received: " + msg);

  if (msg == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED turned ON");
    // confirm back to Node-RED
    mqtt.publish("factory/machine01/status", "{\"led\":1}");
  } else if (msg == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED turned OFF");
    mqtt.publish("factory/machine01/status", "{\"led\":0}");
  }
}

// ── WiFi Connect ─────────────────
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println(" Connected!");
}

// ── MQTT Connect ─────────────────
void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqtt.connect(deviceID)) {
      Serial.println(" Connected!");
      mqtt.subscribe(cmdTopic);   // ← subscribe to commands
      Serial.println("Subscribed to: " + String(cmdTopic));
    } else {
      Serial.println(" Failed. Retrying...");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  dht.begin();

  connectWiFi();
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setCallback(mqttCallback);   // ← register callback
  connectMQTT();

  Serial.println("DHT22 + MQTT started...");
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();   // ← keeps callback alive

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT!");
    delay(2000);
    return;
  }

  Serial.printf("Temp: %.1f°C | Hum: %.1f%%\n", temp, hum);

  StaticJsonDocument<128> doc;
  doc["device"] = deviceID;
  doc["temp"]   = temp;
  doc["hum"]    = hum;

  char payload[128];
  serializeJson(doc, payload);
  mqtt.publish(mqttTopic, payload);
  Serial.println("Published: " + String(payload));

  delay(2000);
}
