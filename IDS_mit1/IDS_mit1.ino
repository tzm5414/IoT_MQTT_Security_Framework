#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Network Settings ---
// Must be set to a static/unique value for the DHCP assignment
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const char* mqtt_server = "192.168.0.100"; 

// --- LCD & LED Settings ---
LiquidCrystal_I2C lcd(0x27, 16, 2); // Use 0x3F if 0x27 fails
const int ledGreen = 6; // Green LED (Secure State)
const int ledRed = 7; // Red LED (Alert State)

EthernetClient ethClient;
PubSubClient client(ethClient);

// This function runs when a message is received (The Detection Logic)
void callback(char* topic, byte* payload, unsigned int length) {
    
    // --- STEP 1: CAPTURE DATA FOR LATENCY (T_alert) ---
    // Prints the millisecond time since the Arduino booted.
    Serial.print("T_alert:");
    Serial.println(millis()); 
    
    // Convert payload to string for LCD display
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    // --- STEP 2: TRIGGER ALERT STATE ---
    digitalWrite(ledRed, HIGH); // Turn on Red LED
    digitalWrite(ledGreen, LOW); // Turn off Green LED
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!ATTACK DETECTED!");
    lcd.setCursor(0, 1);
    lcd.print(message); // Show the attack message (e.g., PICO ATTACK!)
    
    delay(3000); // Show the alert for 3 seconds (Reset Latency)
    
    // --- STEP 3: RESET TO SECURE STATE ---
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Status: Secure");
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ArduinoClient-";
        clientId += String(random(0xffff), HEX);
        
        // BASELINE VULNERABILITY: Connect anonymously
        if (client.connect(clientId.c_str(), "admin", "pass123")) {
            Serial.println("connected!");
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
    Serial.begin(9600); 
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);

    // --- Setup LCD ---
    lcd.init();
    lcd.backlight();
    digitalWrite(ledGreen, HIGH); // Start in secure state
    digitalWrite(ledRed, LOW); 
    lcd.setCursor(0, 0);
    lcd.print("Status: Secure");

    // --- Setup Ethernet ---
    Serial.println("Starting Ethernet (DHCP)...");
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to get IP via DHCP");
        while (true); // Stop here if it fails
    }
    Serial.print("Ethernet IP: ");
    Serial.println(Ethernet.localIP());
    delay(1500); 

    // --- Setup MQTT ---
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop(); 
}