#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN    D8   // Define Slave Select Pin for RC522
#define RST_PIN   D3   // Define Reset Pin for RC522

#define WIFI_SSID "Galaxy A04 c644"
#define WIFI_PASSWORD "lili1234"

#define THINGSPEAK_CHANNEL_ID 2533029   // Replace with your ThingSpeak Channel ID
#define THINGSPEAK_API_KEY "GXCD0ZBT1MO24CDR"  // Replace with your ThingSpeak Write API Key

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Define your RFID card IDs and corresponding names
byte CARD_1_ID[] = {0x21, 0x94, 0x36, 0x1D};
byte CARD_2_ID[] = {0xC1, 0x90, 0x0B, 0x24};

// Corresponding names for the cards
const char* card1Name = "STUDENT OF CLASS 4";
const char* card2Name = "DEPUTY OF STUDIES";

WiFiClient client; // Define the WiFi client

void setup() {
  Serial.begin(115200); // Initialize serial communication

  SPI.begin(); // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522

  connectWiFi();

  ThingSpeak.begin(client); // Initialize ThingSpeak with the client
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String tagName = getTagName(mfrc522.uid.uidByte, mfrc522.uid.size);

    if (tagName != "Unknown") {
      Serial.print("Attendance recorded for: ");
      Serial.println(tagName);

      updateAttendance(tagName);
    } else {
      Serial.println("Unknown card detected. Attendance not recorded.");
    }

    delay(1000); // Delay to avoid multiple reads of the same card
  }
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
}

String getTagName(byte *uid, byte size) {
  if (memcmp(uid, CARD_1_ID, sizeof(CARD_1_ID)) == 0) {
    return card1Name;
  } else if (memcmp(uid, CARD_2_ID, sizeof(CARD_2_ID)) == 0) {
    return card2Name;
  } else {
    return "Unknown";
  }
}

void updateAttendance(String tagName) {
  int fieldNumber = 1; // Field number in ThingSpeak channel

  // Update ThingSpeak channel with tag name
  ThingSpeak.writeField(THINGSPEAK_CHANNEL_ID, fieldNumber, tagName, THINGSPEAK_API_KEY);

  Serial.println("Attendance recorded on ThingSpeak");
}