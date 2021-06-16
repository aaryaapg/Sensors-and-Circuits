/* 
Uses RFID to scan the tag and get the login time. 
A push button is used to trigger logout procedure. 
Duration between login and logout is calculated.
___________________________________Connections______________________________________ 
RFID RC522    ESP32
SDA  -------> 15         
SCK  -------> 18
MOSI -------> 23
MISO -------> 19
IRQ  -------> NC
GND  -------> GND
RST  -------> 27
VCC  -------> 3.3V

Pushbutton -> 22
*/
/* ______________________________________Libraries______________________________________ */
//RFID
#include <SPI.h>
#include <MFRC522.h>
//Timestamp
#include <Time.h> 
#include <TimeLib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
//WiFi
#include <WiFi.h>
/* ______________________________________Macros______________________________________ */
//RFID
#define SS_PIN 15
#define RST_PIN 27
//Wifi
const char* ssid     = "********";
const char* password = "********";
//Button
#define buttonPin 22
/* ______________________________________Declarations and Variables______________________________________ */
//RFID
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
byte uid[4]; // Init array that will store UID
//Timestamp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP); 
long int loginTime = 0;
long int logoutTime = 0;
double duration;
String timeStamp, dateStamp, dayStamp;
//Button
int buttonState; //Logout will be initiated if button state is HIGH
int statuss = 0;
/* ______________________________________Setup______________________________________ */
void setup() {
  Serial.begin(115200);
  //Wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  //Print dots as long as it is not connected
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  //RFID
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  //timeClient.begin();
}

void loop() {
if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
 for (byte i = 0; i < 4; i++) {
      uid[i] = rfid.uid.uidByte[i];
 }
 printHex(rfid.uid.uidByte, rfid.uid.size);
 Serial.println();
 rfid.PICC_HaltA();
 rfid.PCD_StopCrypto1();
 timeClient.update();
 loginTime = timeClient.getEpochTime() + (19800); //Set offset wrt your Timezone - 5 hours 30 minutes = 19800s (India is GMT+5:30)
 getDetailedTimeStamp(loginTime);
 Serial.print("Date: "); Serial.println(dateStamp);
 Serial.print("Day: "); Serial.println(dayStamp);
 Serial.print("Time: "); Serial.println(timeStamp);
 rfid.PCD_AntennaOff();
 Serial.println("Antenna Off "); //Successfully logged in
 statuss=1; //This indicates that an RFID card has been logged in. The sensors will be read as long as statuss=1
}
 if(statuss){
  while(1){
    buttonState = digitalRead(buttonPin);
    if(buttonState==HIGH){ //Logout
      statuss=0;
      timeClient.update();
      logoutTime = timeClient.getEpochTime() + (19800); //5 hours 30 minutes = 19800s (India is GMT+5:30)
      getDetailedTimeStamp(logoutTime);
      duration = ((hour(logoutTime) - hour(loginTime))*3600 + (minute(logoutTime) - minute(loginTime))*60 + (second(logoutTime) - second(loginTime)));
      Serial.print("Duration (between login and logout time) is ");
      Serial.print(duration);
      Serial.println(" seconds.");
      rfid.PCD_AntennaOn();
      Serial.println("Antenna is on");
      break;
    }
  }
 }
}

/* ______________________________________Functions______________________________________ */
//Function to convert epoch time into a readable format
void getDetailedTimeStamp(int logs) {
  timeStamp = String(hour(logs)) + ":" + String(minute(logs)) + ":" + String(second(logs));
  dateStamp = String(day(logs))+ "-" + String(month(logs)) + "-" + String(year(logs));
  switch(weekday(logs))
  {
    case 1:  dayStamp = "Sunday"; break;
    case 2:  dayStamp = "Monday"; break;
    case 3:  dayStamp = "Tuesday"; break;
    case 4:  dayStamp = "Wednesday"; break;
    case 5:  dayStamp = "Thursday"; break;
    case 6:  dayStamp = "Friday"; break;
    default: dayStamp = "Saturday";
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
//https://www.teachmemicro.com/arduino-rfid-rc522-tutorial/
