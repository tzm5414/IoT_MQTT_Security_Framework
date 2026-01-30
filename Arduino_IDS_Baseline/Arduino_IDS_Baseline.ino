#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// --- CONFIGURATION ---
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 0, 100); // Your Laptop IP

// --- PINS ---
const int alertLed = 8;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27 is standard

EthernetClient ethClient;
PubSubClient client(ethClient);

void setup() {
  Serial.begin(9600);
  pinMode(alertLed, OUTPUT);
  digitalWrite(alertLed, LOW); // System Safe

  // Init LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("IDS SYSTEM:");
  lcd.setCursor(0,1);
  lcd.print("MONITORING...");

  // Init Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Try to configure using IP address instead of DHCP:
    IPAddress ip(192, 168, 0, 105); // Fallback IP
    Ethernet.begin(mac, ip);
  }
  delay(1500); // Allow hardware to initialize

  client.setServer(server, 1883);
  client.setCallback(callback);
}

// --- IDS DETECTION LOGIC ---
void callback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to String
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // 1. ANOMALY DETECTION
  if (message.indexOf("PICO ATTACK!") >= 0) {
    triggerAlert(topic, message);
  }
}

void triggerAlert(char* topic, String msg) {
  Serial.println("!!! ALERT: INTRUSION DETECTED !!!");
  Serial.print("Source Topic: "); Serial.println(topic);
  Serial.print("Payload: "); Serial.println(msg);

  // PHYSICAL RESPONSE
  digitalWrite(alertLed, HIGH); // Red LED ON
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("!ATTACK DETECTED");
  lcd.setCursor(0,1);
  lcd.print("Payload: MALWARE");
  
  delay(5000); // Hold alert for 5 seconds
  
  // Reset System State
  digitalWrite(alertLed, LOW);
  lcd.clear();
  lcd.print("IDS SYSTEM:");
  lcd.setCursor(0,1);
  lcd.print("MONITORING...");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("IDS connecting...");
    // Connect anonymously for Scenario 1
    if (client.connect("ArduinoIDS")) {
      Serial.println("connected");
      client.subscribe("#"); // WILDCARD: Listen to EVERYTHING
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
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