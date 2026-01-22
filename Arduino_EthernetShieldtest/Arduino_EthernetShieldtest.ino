#include <SPI.h>
#include <Ethernet.h>

// --- Network Settings ---
// Enter a MAC address for your controller.
// This is a FAKE MAC address. It just needs to be unique on your network.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

void setup() {
  // Start the Serial Monitor
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Attempting to get an IP address using DHCP...");

  // Start the Ethernet connection using DHCP 
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Serial.println("Please check your Ethernet cable.");
    // Stop here if it failed
    while (true) {
      ;
    }
  }

  // If we get here, it was successful!
  Serial.println("DHCP connection SUCCESSFUL!");
  Serial.print("My IP address is: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Nothing to do in the loop
}