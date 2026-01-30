#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURATION ---
const char* ssid = "IsolatedNW2.4Ghz";  // Your Router Name
const char* password = "39173096";      // Your Router Password
const char* mqtt_server = "192.168.0.100"; // Your Laptop IP

// --- PIN DEFINITIONS ---
const int ledPin = D5; // The "Factory Actuator"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Start with Light OFF (Safe state)

  setup_wifi();
  client.setServer(mqtt_server, 1883); // Standard Insecure Port
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// --- THIS IS WHERE THE ATTACK HAPPENS ---
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // If the PICO sends this message, the system is HACKED
  if (message == "PICO ATTACK!") {
    Serial.println(">>> CRITICAL: UNAUTHORIZED COMMAND EXECUTED <<<");
    // Blink Pattern to show 'Hacked' status
    for(int i=0; i<5; i++){
      digitalWrite(ledPin, HIGH); delay(100);
      digitalWrite(ledPin, LOW); delay(100);
    }
    digitalWrite(ledPin, HIGH); // Leave light ON (Compromised)
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect (NO PASSWORD for Baseline)
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("thant-thesis-test/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
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