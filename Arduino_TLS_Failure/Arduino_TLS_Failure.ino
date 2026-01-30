#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Note: Standard EthernetClient DOES NOT support SSL.
// Attempting to connect to Port 8883 (SSL) with a non-SSL client
// will result in immediate disconnection or hanging.

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 0, 100);

EthernetClient ethClient;
PubSubClient client(ethClient);

void setup() {
  Serial.begin(9600);
  if (Ethernet.begin(mac) == 0) {
    IPAddress ip(192, 168, 0, 105);
    Ethernet.begin(mac, ip);
  }
  delay(1500);

  // --- ATTEMPTING SECURE PORT 8883 ---
  Serial.println("ATTEMPTING TLS ON ARDUINO UNO (8-BIT)...");
  client.setServer(server, 8883); 
}

void loop() {
  if (!client.connected()) {
    Serial.print("Connecting to 8883...");
    // This will hang or fail instantly because the handshake cannot complete
    if (client.connect("ArduinoFail", "admin", "pass123")) {
      Serial.println("Connected (IMPOSSIBLE)");
    } else {
      Serial.print("FAILED! rc=");
      Serial.print(client.state());
      Serial.println(" [Hardware Resource Exhaustion]");
      delay(5000);
    }
  }
}