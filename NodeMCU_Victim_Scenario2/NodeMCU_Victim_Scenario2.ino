#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURATION ---
const char* ssid = "IsolatedNW2.4Ghz";
const char* password = "39173096";
const char* mqtt_server = "192.168.0.100";

// --- NEW FOR SCENARIO 2: AUTHENTICATION ---
// The Victim acts as a legitimate device, so we give it Admin creds
const char* mqtt_user = "admin";
const char* mqtt_pass = "pass123";

const int ledPin = D5;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883); // Still Port 1883
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
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Msg: "); Serial.println(message);

  if (message == "PICO ATTACK!") {
    Serial.println(">>> UNAUTHORIZED COMMAND EXECUTED <<<");
    for(int i=0; i<5; i++){
      digitalWrite(ledPin, HIGH); delay(100);
      digitalWrite(ledPin, LOW); delay(100);
    }
    digitalWrite(ledPin, HIGH); 
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting Authenticated connection...");
    String clientId = "ESP8266Client-Victim";
    
    // --- CHANGE: NOW WE SEND USERNAME AND PASSWORD ---
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
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