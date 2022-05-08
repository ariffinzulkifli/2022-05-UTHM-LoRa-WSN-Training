#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(923E6)) {
    Serial.println("Initializing LoRa failed!");
    while (1);
  }
}

void loop() {
  // Try to parse incoming packet
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    // Received a packet
    Serial.print("Received Packet '");

    // read packet
    String packet;
    while (LoRa.available()) {
      packet += LoRa.read();
    }
    Serual.print(packet);

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
