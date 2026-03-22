# Smart Factory Telemetry Node (ESP32 + MQTT)

An Industrial IoT (IIoT) reference implementation using an ESP32 to monitor machine health. This project captures environmental data and 6-axis motion data to detect anomalies in factory equipment.

##  Overview
This node collects temperature, humidity, and vibration data (Acceleration + Gyroscope), packages it into a structured **JSON payload**, and publishes it to an **MQTT Broker** for real-time monitoring and analytics.

### Key Features
* **Real-time Telemetry:** Periodic updates via HiveMQ public broker.
* **Sensor Fusion:** Combines DHT22 (Ambient) and MPU6050 (Inertial) data.
* **Robust Connectivity:** Automatic reconnection logic for WiFi and MQTT.
* **Standardized Messaging:** Uses `ArduinoJson` for lightweight, interoperable data exchange.

## Hardware Components
* **Microcontroller:** ESP32 (DevKit V1)
* **Sensors:** * DHT22 (Temperature & Humidity)
    * MPU6050 (3-Axis Accelerometer & 3-Axis Gyroscope)
* **Protocol:** MQTT over WiFi

## Pin Mapping
| Sensor | ESP32 Pin | Protocol |
| :--- | :--- | :--- |
| **DHT22 Data** | GPIO 4 | One-Wire |
| **MPU6050 SDA** | GPIO 21 | I2C |
| **MPU6050 SCL** | GPIO 22 | I2C |

## Software Setup
### Libraries Required
Ensure you have the following libraries installed in your Arduino IDE or PlatformIO:
1. `PubSubClient` by Nick O'Leary
2. `DHT sensor library` by Adafruit
3. `Adafruit MPU6050` by Adafruit
4. `ArduinoJson` by Benoit Blanchon

### Configuration
Update the `ssid` and `password` variables in the code to match your network credentials. By default, it is configured for the **Wokwi Simulation** environment.

## Data Schema
Messages are published to `factory/machine01/data` in the following format:
```json
{
  "device": "MACHINE-01",
  "temp": 24.5,
  "hum": 50.2,
  "accel_x": 0.12,
  "accel_y": -0.05,
  "accel_z": 9.81,
  "gyro_x": 0.01,
  "gyro_y": 0.02,
  "gyro_z": -0.01
}
