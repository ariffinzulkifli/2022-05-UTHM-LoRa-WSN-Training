#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <MQTT.h>
#include <Adafruit_SSD1306.h>

// TTGO LoRa32 Pins Definition
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

// TTGO LoRa32 OLED Definition
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi and Favoriot Credentials
const char ssid[] = "YourSSID";
const char password[] = "YourWiFiPassword";
const char deviceAccessToken[] = "YourDeviceAccessToken";
const char deviceDeveloperId[] = "YourDeviceDeveloperId";
const char publishTopic[] = "/v2/streams";
const char statusTopic[] = "/v2/streams/status";

WiFiClient net;
MQTTClient mqtt(1024);

unsigned long lastMillis = 0;

void connectToWiFi() {
  
  Serial.println();
  Serial.print("Initialize LoRa Gateway: ");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println(F("SSD1306 allocation failed"));
  display.clearDisplay();
  
  Serial.print("Connecting to Wi-Fi '" + String(ssid) + "' ...");

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println(" connected!");

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

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming Status: " + payload);
  Serial.println();
}

void connectToFavoriotMQTT(){
  Serial.print("Connecting to Favoriot MQTT ...");
  
  mqtt.begin("mqtt.favoriot.com", net);
  mqtt.onMessage(messageReceived);

  String uniqueString = String(ssid) + "-" + String(random(1, 98)) + String(random(99, 999));
  char uniqueClientID[uniqueString.length() + 1];
  
  uniqueString.toCharArray(uniqueClientID, uniqueString.length() + 1);
  
  while (!mqtt.connect(uniqueClientID, deviceAccessToken, deviceAccessToken)) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" connected!");

  Serial.println("Subscribe to: " + String(deviceAccessToken) + String(statusTopic));
  
  mqtt.subscribe(String(deviceAccessToken) + String(statusTopic));
  Serial.println();
}

void setup() {
  Serial.begin(9600);

  Serial.println("LoRa Gateway");

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("> Starting LoRa failed!");
    while(1);
  }

  // LoRa.setSyncWord(LORA_SYNC_WORD); // default 0x34
  LoRa.setSpreadingFactor(LORA_SPREADING);

  connectToWiFi();
  connectToFavoriotMQTT();
}

void loop() {

  mqtt.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (!mqtt.connected()) {
    connectToFavoriotMQTT();
  }
  
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
    Serial.print(packet);
    
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    display.setCursor(0, 30);
    display.println("> " + packet);

    display.print("> RSSI: " + String(LoRa.packetRssi())  + " dBm");
    display.display();

    // publish a packet to Favoriot
    String favoriotJson = "{";
    favoriotJson += "\"device_developer_id\":\"" + String(deviceDeveloperId) + "\",";
    favoriotJson += "\"data\":{";
    favoriotJson += "\"ph\":" + packet + ",";
    favoriotJson += "}";
    favoriotJson += "}";

    Serial.println("Data to Publish: " + favoriotJson);
    Serial.println("Publish to: " + String(deviceAccessToken) + String(publishTopic));
    
    mqtt.publish(String(deviceAccessToken) + String(publishTopic), favoriotJson);

    Serial.println("Packet sent to Favoriot");
    Serial.println();
  }
}
