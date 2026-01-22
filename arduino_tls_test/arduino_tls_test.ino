#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- NETWORK SETTINGS ---
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const char* mqtt_server = "192.168.0.100"; 

// --- AUTHENTICATION (Required for the secure broker) ---
const char* username = "admin";
const char* password = "pass123";

// --- LCD & LED Settings ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 
const int ledGreen = 6; 
const int ledRed = 7;

EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
    // (This will likely never run in this test, because connection will fail)
    Serial.print("T_alert:");
    Serial.println(millis()); 
    
    // ... Alert Logic ...
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW); 
    lcd.clear();
    lcd.print("!ATTACK DETECTED!");
    delay(3000); 
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
    lcd.clear();
    lcd.print("Status: Secure");
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting TLS connection on Port 8883...");
        String clientId = "ArduinoClient-";
        clientId += String(random(0xffff), HEX);
        
        // --- THE TEST ---
        // This tries to connect to the encrypted port. 
        // Since Arduino UNO cannot handle TLS, this connection will hang or fail.
        if (client.connect(clientId.c_str(), username, password)) { 
            Serial.println("CONNECTED!"); // You will likely NOT see this
            client.subscribe("thant-thesis-test/control");
        } else {
            // THIS IS THE RESULT YOU WANT TO RECORD
            Serial.print("Failed! rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(9600); 
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    lcd.init();
    lcd.backlight();
    
    // Ethernet Setup
    if (Ethernet.begin(mac) == 0) { while (true); }
    delay(1500); 

    // --- TARGETING SECURE PORT ---
    client.setServer(mqtt_server, 8883); // Target Port 8883
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop(); 
}