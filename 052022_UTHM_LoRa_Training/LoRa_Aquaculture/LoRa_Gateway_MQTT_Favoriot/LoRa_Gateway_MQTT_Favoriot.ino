#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  Serial.println();
  Serial.print("Initialize LoRa Gateway: ");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println(F("SSD1306 allocation failed"));

  display.clearDisplay();

  // display.setRotation(2);
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0, 0);
  display.print("Myinvent LoRa: ");
  display.display();

  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(18, 23, 26);

  if (!LoRa.begin(923E6)) {
    Serial.println("NOT OK");
    display.println("NOT OK");
    display.display();
    while (1);
  }

  Serial.println("OK");
  display.print("OK");

  display.setCursor(0, 20);
  display.print("Received Packet:");
  display.display();
  
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
      packet += (char)LoRa.read();
    }
    Serial.print(packet);
    
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    display.setCursor(0, 30);
    display.println("> " + packet);

    display.print("> RSSI: " + String(LoRa.packetRssi())  + " dBm");
    display.display();
  }
}
