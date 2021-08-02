/*
Connections
RC522         Uno
SDA  -------> Digital 10
SCK  -------> Digital 13
MOSI -------> Digital 11
MISO -------> Digital 12
IRQ  -------> unconnected
GND  -------> GND
RST  -------> Digital 9
3.3V -------> 3.3V        
*/

/* ______________________________________Libraries______________________________________ */
//RFID
#include <SPI.h>
#include<MFRC522.h>
/* _______________________________________Macros________________________________________ */
//RFID
#define SS_PIN 10
#define RST_PIN 9
/* _____________________________Declarations_and_Variables______________________________ */
//RFID
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
byte uid[4]; // Init array that will store UID
String RFIDCode = ""; 
/* ______________________________________Functions______________________________________ */
/* ________________________________________Setup________________________________________ */
void setup() {
  Serial.begin(9600);
  //RFID
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}
/* ________________________________________Loop_________________________________________ */
void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      uid[i] = rfid.uid.uidByte[i];
    }
    //Get RFID card
    for (byte j = 0; j < rfid.uid.size; j++) {
      RFIDCode.concat(String(rfid.uid.uidByte[j] < 0x10 ? "0" : ""));
      RFIDCode.concat(String(rfid.uid.uidByte[j], HEX));
    }
    //Print RFID Tag
    RFIDCode.toUpperCase();
    RFIDCode=String(RFIDCode);
    Serial.print("RFID Code: "); Serial.println(RFIDCode);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    //Turn Antenna off
    rfid.PCD_AntennaOff(); 
  }
  else {
    //Serial.println("Please Scan");
  }
}
