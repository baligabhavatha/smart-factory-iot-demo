#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// WiFi
const char* ssid = <WIFI Username>;
const char* password = <WIFI Password>;

// MQTT
const char* mqtt_server = "broker.hivemq.com";

// Topics
const char* pub_topic = "esp32/dht11/bhavatha";
const char* sub_topic = "esp32/led/control";

// DHT setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED
#define LED_PIN 2   // Use GPIO 2 (built-in LED for many ESP32 boards)

WiFiClient espClient;
PubSubClient client(espClient);

// Callback when message is received
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  Serial.print("Message: ");
  Serial.println(msg);

  // LED Control
  if (msg == "ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
  } 
  else if (msg == "OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
  }
}

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

      // Subscribe after connection
      client.subscribe(sub_topic);
      Serial.println("Subscribed to LED topic");
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

  pinMode(LED_PIN, OUTPUT);  // LED setup

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  // Set callback
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(" Failed to read from DHT!");
    delay(2000);
    return;
  }

  // JSON payload
  String payload = "{";
  payload += "\"temp\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity);
  payload += "}";

  Serial.println(payload);

  // Publish sensor data
  if (client.publish(pub_topic, payload.c_str())) {
    Serial.println("Published to MQTT");
  } else {
    Serial.println("Publish failed");
  }

  delay(3000);
}
const char* password = <wifi password>;

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
