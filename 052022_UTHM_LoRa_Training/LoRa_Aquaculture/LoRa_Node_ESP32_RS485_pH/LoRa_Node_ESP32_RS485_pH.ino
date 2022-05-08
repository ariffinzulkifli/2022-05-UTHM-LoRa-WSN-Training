#include <SPI.h>
#include <LoRa.h>

unsigned long lastSend = 0;

void setup() {
  Serial.begin(9600);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(923E6)) {
    Serial.println("Initializing LoRa failed!");
    while (1);
  }
}

void loop() {

  // Sensor's data acquisition
  

  // Send to LoRa Gateway with given interval time
  if(millis() - lastSend > 3000){
    // send packet
    LoRa.beginPacket();
    LoRa.print();
    LoRa.endPacket();
  }
}
