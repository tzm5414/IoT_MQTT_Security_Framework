#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <PubSubClient.h>
#include <time.h>             
#include <sys/time.h>         // Required for manual time setting
#include "certificate.h"      // Loads your CA Key

// --- NETWORK CONFIGURATION ---
const char* ssid = "IsolatedNW2.4Ghz";
const char* password = "39173096"; 
const char* mqtt_server = "192.168.0.100"; // Your Laptop IP

// --- AUTHENTICATION ---
const char* username = "admin";
const char* pass = "pass123";

// --- HARDWARE CONFIGURATION ---
const int ledPin = D5; // Pin D5 (GPIO14)

// --- OBJECTS ---
WiFiClientSecure espClient; 
PubSubClient client(espClient);

// --- CALLBACK (ACTUATION LOGIC) ---
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Attack Logic
  Serial.println("COMMAND RECEIVED: Turning light OFF");
  digitalWrite(ledPin, LOW); 
}

// --- RECONNECT LOGIC ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting TLS connection on 8883...");
    
    // Create random Client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt Connect
    if (client.connect(clientId.c_str(), username, pass)) { 
      Serial.println("CONNECTED (TLS)!");
      client.subscribe("thant-thesis-test/control"); 
    } else {
      Serial.print("TLS ERROR - rc=");
      Serial.print(client.state());
      
      // Print SSL error for debugging
      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print(" SSL Detail: ");
      Serial.println(buf);
      
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(9600); 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Default ON
  
  // 1. Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  Serial.println("\nWiFi Connected");

  // 2. MANUAL TIME SET (OFFLINE MODE)
  // We force the ESP to think it is Dec 2025 so the certificate is valid.
  // This bypasses the need for Internet/NTP.
  Serial.println("Setting manual time (Offline Mode)...");
  
  struct timeval tv;
  tv.tv_sec = 1766800000; // Epoch Timestamp for late 2025
  tv.tv_usec = 0;
  settimeofday(&tv, nullptr); 

  // Verify the time was set
  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current Time: ");
  Serial.print(asctime(&timeinfo));

  // 3. LOAD CERTIFICATE (CHAIN OF TRUST)
  espClient.setTrustAnchors(new X509List(test_root_ca)); 
  
  // 4. MQTT Server Setup
  client.setServer(mqtt_server, 8883); 
  client.setCallback(callback);
}

// --- LOOP ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 
}