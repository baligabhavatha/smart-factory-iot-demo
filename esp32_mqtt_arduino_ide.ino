#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// WiFi
const char* ssid = "IBMGuest";
const char* password = "Ebay-4-Puppy-9-Line";

// MQTT
const char* mqtt_server = "broker.hivemq.com";

// DHT setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_DHT11")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT!");
    delay(2000);
    return;
  }

  // JSON payload
  String payload = "{";
  payload += "\"temp\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity);
  payload += "}";

  Serial.println(payload);

  // Publish
  //client.publish("esp32/dht11", payload.c_str());
  if (client.publish("esp32/dht11/bhavatha", payload.c_str())) {
  Serial.println("Published to MQTT");
} else {
  Serial.println(" Publish failed");
}

  delay(3000);
}
