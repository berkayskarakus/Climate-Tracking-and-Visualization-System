# ESP32 DHT11 ThingSpeak Monitor

[![Platform](https://img.shields.io/badge/platform-ESP32-blue)](https://www.espressif.com/en/products/socs/esp32)
[![Language](https://img.shields.io/badge/language-C++-blue)](https://en.wikipedia.org/wiki/C%2B%2B)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

This project reads temperature and humidity from a DHT11 sensor using an ESP32 and uploads the data to **ThingSpeak** IoT analytics platform. The ESP32 reads the sensor every 2 seconds and sends data to ThingSpeak every 15 seconds.

## Features

- ✅ Reads DHT11 temperature & humidity  
- ✅ Automatically reconnects to WiFi if connection drops  
- ✅ Sends data to ThingSpeak channel (fields 1 & 2)  
- ✅ Serial monitor commands for manual control  
- ✅ Self‑diagnostic system status output  
- ✅ Error recovery for sensor read failures  

## Hardware Required

| Component          | Quantity |
|--------------------|----------|
| ESP32 development board | 1 |
| DHT11 sensor       | 1 |
| 10kΩ resistor (pull‑up) | 1 |
| Breadboard & jumper wires | as needed |

## Wiring Diagram

| DHT11 Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | 3.3V or 5V |
| GND       | GND        |
| DATA      | GPIO4      |

> **Important:** Add a 10kΩ pull‑up resistor between DATA and VCC.

## Software Dependencies

Install the following libraries via Arduino Library Manager:

- [`DHT sensor library`](https://github.com/adafruit/DHT-sensor-library) by Adafruit  
- [`ThingSpeak`](https://github.com/mathworks/thingspeak-arduino) by MathWorks  
- Built‑in `WiFi.h` (comes with ESP32 board package)

## Configuration

Edit the top of the `.ino` file:

```cpp
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASS = "YourWiFiPassword";

const unsigned long THINGSPEAK_CHANNEL = 1234567;   // your channel ID
const char* THINGSPEAK_API_KEY = "YOUR_API_KEY";   // Write API Key

#define DHTPIN 4        // change if using another GPIO
