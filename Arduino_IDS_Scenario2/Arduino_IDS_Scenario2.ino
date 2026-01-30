#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// --- CONFIGURATION ---
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 0, 100);

// --- NEW FOR SCENARIO 2: AUTHENTICATION ---
const char* mqtt_user = "admin";
const char* mqtt_pass = "pass123";

const int alertLed = 8;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

EthernetClient ethClient;
PubSubClient client(ethClient);

void setup() {
  Serial.begin(9600);
  pinMode(alertLed, OUTPUT);
  digitalWrite(alertLed, LOW); 

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("IDS (SECURE):");
  lcd.setCursor(0,1);
  lcd.print("MONITORING...");

  if (Ethernet.begin(mac) == 0) {
    IPAddress ip(192, 168, 0, 105);
    Ethernet.begin(mac, ip);
  }
  delay(1500); 

  client.setServer(server, 1883); // Still Port 1883
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // ANOMALY DETECTION
  if (message.indexOf("PICO ATTACK!") >= 0) {
    triggerAlert(topic, message);
  }
}

void triggerAlert(char* topic, String msg) {
  Serial.println("!!! ALERT: INTRUSION DETECTED !!!");
  digitalWrite(alertLed, HIGH); 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("!ATTACK DETECTED");
  delay(5000); 
  
  digitalWrite(alertLed, LOW);
  lcd.clear();
  lcd.print("IDS (SECURE):");
  lcd.setCursor(0,1);
  lcd.print("MONITORING...");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("IDS connecting...");
    // --- CHANGE: NOW WE SEND USERNAME AND PASSWORD ---
    if (client.connect("ArduinoIDS", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe("#"); 
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