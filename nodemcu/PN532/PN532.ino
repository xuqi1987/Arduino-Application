#include <ESP8266WiFi.h>
//#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// NFC module
#define PN532_SCK   14
#define PN532_MOSI  12
#define PN532_MISO  13
#define PN532_SS    15
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// forward declaration of helper function to get UID as HEX-String
void byteToHexString(String &dataString, byte *uidBuffer, byte bufferSize, String strSeperator);

void setup() {
  Serial.begin(9600); 

  Serial.println("Initial nfc module");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A card");
  Serial.println("Initial tft display");

}

void loop(void) {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  //success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength,25);

  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    String strUID;
    // store UID as HEX-String to strUID and print it to display
    byteToHexString(strUID, uid, uidLength, "-");
    Serial.println("");
    Serial.println(strUID);
    // Wait 1 second before continuing
    delay(1000);
  }
  else
  {
    Serial.println("Timed out or waiting for a card");
  }
}

// helper function to get UID as HEX-String
void byteToHexString(String &dataString, byte *uidBuffer, byte bufferSize, String strSeperator=":") {
  dataString = "";
  for (byte i = 0; i < bufferSize; i++) {
    if (i>0) {
      dataString += strSeperator;
      if (uidBuffer[i] < 0x10)
        dataString += String("0");
    }
    dataString += String(uidBuffer[i], HEX);
  }
  dataString.toUpperCase();
}
