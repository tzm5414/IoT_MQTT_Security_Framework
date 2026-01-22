#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --- NETWORK CONFIGURATION (ISOLATED LAB) ---
// This is the vulnerable setup (targets 192.168.0.100 broker)
const char* ssid = "IsolatedNW2.4Ghz";
const char* password = "39173096"; 
const char* mqtt_server = "192.168.0.100"; 
// -------------------------------------------

// --- VICTIM ACTUATOR ---
const int ledPin = D5; // Pin D5 (or GPIO14) where your Blue LED is connected.

WiFiClient espClient;
PubSubClient client(espClient);

// This function runs when a message is received
void callback(char* topic, byte* payload, unsigned int length) {
  // Serial Log (for debugging purposes)
  Serial.print("Attack Payload Received on [");
  Serial.print(topic);
  Serial.print("]: ");
  
  // --- VICTIM ACTUATION LOGIC ---
  // The attack is successful! Turn the light OFF (Compromised state).
  Serial.println("COMPROMISE: Turning light OFF.");
  digitalWrite(ledPin, LOW); 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // BASELINE VULNERABILITY: Attempting ANONYMOUS connection (no user/pass)
    if (client.connect(clientId.c_str())) {
      Serial.println("connected!");
      // Subscribe to the control topic
      client.subscribe("thant-thesis-test/control"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600); // Set baud rate for reliable viewing
  delay(10);
  
  // --- HARDWARE SETUP ---
  pinMode(ledPin, OUTPUT);
  // Turn the light ON immediately to show we are in the secure, operational state
  digitalWrite(ledPin, HIGH); 
  
  // --- WI-FI CONNECTION ---
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  // --- MQTT SETUP ---
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Required to maintain connection and process incoming messages
}