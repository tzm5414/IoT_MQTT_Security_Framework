#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
#include <time.h>             
#include <sys/time.h>         
#include "certificate.h"      // YOUR CERTIFICATE FILE

const char* ssid = "IsolatedNW2.4Ghz";
const char* password = "39173096"; 
const char* mqtt_server = "192.168.0.100"; 

// CREDENTIALS (Authorized)
const char* mqtt_user = "admin";
const char* mqtt_pass = "pass123";

const int ledPin = D5; 

WiFiClientSecure espClient; 
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  Serial.println("\nWiFi Connected");

  // --- TIME TRAVEL FIX (FOR TLS) ---
  struct timeval tv;
  tv.tv_sec = 1766800000; // Dec 2025
  tv.tv_usec = 0;
  settimeofday(&tv, nullptr); 

  // --- LOAD CERTIFICATE ---
  espClient.setTrustAnchors(new X509List(test_root_ca)); 
  
  // --- CONNECT TO SECURE PORT 8883 ---
  client.setServer(mqtt_server, 8883); 
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  // IF ATTACK SUCCEEDS (Which it shouldn't)
  if (message == "PICO ATTACK!") {
    digitalWrite(ledPin, HIGH); 
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting SECURE TLS connection...");
    unsigned long start = millis(); // Measure Latency
    
    if (client.connect("ESP8266-Secure", mqtt_user, mqtt_pass)) { 
      unsigned long end = millis();
      Serial.print("Connected! Latency: ");
      Serial.print(end - start);
      Serial.println(" ms"); // SHOW THIS TO EXAMINER (Should be ~1800ms)
      
      client.subscribe("thant-thesis-test/control"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print(" SSL Error: ");
      Serial.println(buf);
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 
}