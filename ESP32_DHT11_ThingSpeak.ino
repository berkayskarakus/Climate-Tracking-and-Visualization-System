/*
 * ESP32 DHT11 - ThingSpeak Data Upload
 * 
 * Reads DHT11 every 2 seconds
 * Sends to ThingSpeak every 15 seconds
 * 
 * Connections:
 * DHT11 VCC  -> ESP32 3.3V or 5V
 * DHT11 GND  -> ESP32 GND
 * DHT11 DATA -> GPIO4 (or any other GPIO)
 */

#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

// ==================== CONFIGURATION ====================
// WiFi credentials
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASS = "YourWiFiPassword";

// ThingSpeak settings
const unsigned long THINGSPEAK_CHANNEL = 1234567;        // Replace with your channel ID
const char* THINGSPEAK_API_KEY = "YOUR_API_KEY";        // Write API key

// DHT11 sensor settings
#define DHTPIN 4          // GPIO pin connected to DHT11 data
#define DHTTYPE DHT11

// Timing intervals (milliseconds)
const unsigned long SENSOR_READ_INTERVAL = 2000;     // 2 seconds
const unsigned long THINGSPEAK_SEND_INTERVAL = 15000; // 15 seconds

// ==================== GLOBAL VARIABLES ====================
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

float temperature = 0;
float humidity = 0;

unsigned long lastSensorRead = 0;
unsigned long lastThingSpeakSend = 0;

bool wifiConnected = false;
bool dhtInitialized = false;

// ==================== FUNCTION DEFINITIONS ====================

/**
 * Connects to WiFi network
 * @return true if connection succeeded, false otherwise
 */
bool connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
    return true;
  } else {
    Serial.println("\n❌ WiFi connection failed!");
    wifiConnected = false;
    return false;
  }
}

/**
 * Reads temperature and humidity from DHT11 sensor
 * @return true if read succeeded, false otherwise
 */
bool readDHT11() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius
  
  if (isnan(h) || isnan(t)) {
    Serial.println("❌ DHT11 read error!");
    return false;
  }
  
  temperature = t;
  humidity = h;
  
  Serial.print("📊 DHT11: ");
  Serial.print(temperature, 1);
  Serial.print("°C, ");
  Serial.print(humidity, 1);
  Serial.println("%");
  
  return true;
}

/**
 * Sends current temperature and humidity to ThingSpeak channel
 * @return true if send succeeded, false otherwise
 */
bool sendToThingSpeak() {
  if (!wifiConnected) {
    Serial.println("Cannot send to ThingSpeak: WiFi not connected");
    return false;
  }
  
  Serial.println("Sending data to ThingSpeak...");
  
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  
  int httpCode = ThingSpeak.writeFields(THINGSPEAK_CHANNEL, THINGSPEAK_API_KEY);
  
  if (httpCode == 200) {
    Serial.println("✅ Data sent to ThingSpeak successfully!");
    return true;
  } else {
    Serial.print("❌ ThingSpeak error! HTTP code: ");
    Serial.println(httpCode);
    return false;
  }
}

/**
 * Prints full system status to Serial monitor
 */
void printSystemStatus() {
  Serial.println("\n" + String(50, '='));
  Serial.println("SYSTEM STATUS");
  Serial.println(String(50, '-'));
  Serial.println("WiFi: " + String(wifiConnected ? "✅ CONNECTED" : "❌ DISCONNECTED"));
  Serial.println("DHT11: " + String(dhtInitialized ? "✅ READY" : "❌ ERROR"));
  Serial.println("Last reading: " + String(temperature, 1) + "°C, " + String(humidity, 1) + "%");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  
  if (wifiConnected) {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  }
  
  Serial.println(String(50, '=') + "\n");
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔══════════════════════════════════════╗");
  Serial.println(  "║      ESP32 DHT11 + ThingSpeak       ║");
  Serial.println(  "╚══════════════════════════════════════╝");
  Serial.println();
  
  Serial.println("Initializing system...");
  Serial.println("DHT11 will read every 2 seconds");
  Serial.println("ThingSpeak will send every 15 seconds");
  Serial.println();
  
  // Initialize DHT11
  Serial.print("Initializing DHT11 sensor (pin ");
  Serial.print(DHTPIN);
  Serial.println(")...");
  dht.begin();
  delay(2000);
  
  // Test DHT11
  if (readDHT11()) {
    dhtInitialized = true;
    Serial.println("✅ DHT11 initialized successfully!");
  } else {
    Serial.println("❌ DHT11 initialization failed!");
    Serial.println("Please check connections:");
    Serial.println("1. DHT11 VCC -> 3.3V or 5V");
    Serial.println("2. DHT11 GND -> GND");
    Serial.println("3. DHT11 DATA -> GPIO" + String(DHTPIN));
    Serial.println("4. Add a 10KΩ pull-up resistor");
  }
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  
  Serial.println("\n✅ System initialization complete!");
  printSystemStatus();
}

// ==================== LOOP ====================
void loop() {
  // Check WiFi connection status
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    Serial.println("WiFi connection lost! Reconnecting...");
    connectWiFi();
  }
  
  // Read DHT11 every 2 seconds
  if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = millis();
    
    if (!readDHT11()) {
      // On error, reset sensor by pulling pin low
      pinMode(DHTPIN, OUTPUT);
      digitalWrite(DHTPIN, LOW);
      delay(100);
      pinMode(DHTPIN, INPUT_PULLUP);
      delay(100);
    }
  }
  
  // Send to ThingSpeak every 15 seconds
  if (millis() - lastThingSpeakSend >= THINGSPEAK_SEND_INTERVAL) {
    lastThingSpeakSend = millis();
    
    if (wifiConnected && dhtInitialized) {
      sendToThingSpeak();
    } else {
      Serial.println("Cannot send to ThingSpeak: conditions not met");
      Serial.println("- WiFi: " + String(wifiConnected ? "Connected" : "Not connected"));
      Serial.println("- DHT11: " + String(dhtInitialized ? "Ready" : "Not ready"));
    }
    
    // Show system status every 5 sends
    static int sendCount = 0;
    sendCount++;
    if (sendCount % 5 == 0) {
      printSystemStatus();
    }
  }
  
  // Small delay to avoid CPU hogging
  delay(100);
}
