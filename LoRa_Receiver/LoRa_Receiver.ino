#include <SPI.h>
#include <LoRa.h>

// TTGO LoRa32 Pin Definition
#define SPI_MOSI            27
#define SPI_MISO            19
#define SPI_SCK             5

#define LORA_RST            23
#define LORA_CS             18

#define LORA_DIO0           26

#define LORA_FREQUENCY      923.0
#define LORA_BANDWIDTH      125.0
#define LORA_SPREADING      7
#define LORA_CODING_RATE    7
#define LORA_OUTPUT_POWER   10
#define LORA_PREAMBLE_LEN   8
#define LORA_GAIN           0

void setup() {
  Serial.begin(9600);

  Serial.println("LoRa Receiver");

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("> Starting LoRa failed!");
    while(1);
  }

  // LoRa.setSyncWord(LORA_SYNC_WORD); // default 0x34
  LoRa.setSpreadingFactor(LORA_SPREADING);
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    String packet;
    while (LoRa.available()) {
      packet += (char)LoRa.read();
    }
    Serial.print(packet);

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
