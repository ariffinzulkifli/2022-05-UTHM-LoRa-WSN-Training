#include <SPI.h>
#include <LoRa.h>

int counter = 0;

// Makerfabs LoRa RS485 Pin Definition
#define SPI_MOSI            13
#define SPI_MISO            12
#define SPI_SCK             14

#define LORA_RST            33
#define LORA_CS             32

#define LORA_DIO0           25
#define LORA_DIO1           27

#define LORA_FREQUENCY      923E6
#define LORA_BANDWIDTH      125.0
#define LORA_SPREADING      7
#define LORA_CODING_RATE    7
#define LORA_OUTPUT_POWER   10
#define LORA_PREAMBLE_LEN   8
#define LORA_GAIN           0

void setup() {
  Serial.begin(115200);

  Serial.println("LoRa Sender");

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
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
