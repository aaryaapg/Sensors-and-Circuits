//Get date, day and time via NTP protoco. Connection to Wifi is required.
/* ______________________________________Libraries______________________________________ */
#include <Arduino.h>
//Wifi
#include <WiFi.h>
//Timestamp
#include <Time.h> 
#include <TimeLib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
/* _______________________________________Macros________________________________________ */
//Wifi
const char* ssid     = "ssid";
const char* password = "pw";
/* _____________________________Declarations_and_Variables______________________________ */
//Timestamp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP); 
long int timeNow = 0;
/* ______________________________________Functions______________________________________ */
//Connecting to a WiFi network
void connectToWifi(){
  Serial.println(); Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  //Print dots as long as it is not connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(); Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  //Getting MAC Address of ESP32
  WiFi.mode(WIFI_MODE_STA);
  Serial.print("MAC address: "); Serial.println(WiFi.macAddress());
}
//Function to append zeros
String appendZero(int az){
  if(az<10){
    return String("0"+String(az));
  }
  else{
    return String(az);
  }
}
//Function to convert epoch time into a readable format
void getDetailedTimeStamp(int logs) {
  String Hr, Min, Sec, Day, Month, Year;
  String timeStamp, dateStamp, dayStamp;
  //Send each to the appendZero function to append zeros if needed
  Hr = appendZero(hour(logs));
  Min = appendZero(minute(logs));
  Sec = appendZero(second(logs));
  Day = appendZero(day(logs));
  Month = appendZero(month(logs));
  Year = String(year(logs));
  //Create timestamp
  timeStamp = Hr + ":" + Min + ":" + Sec;
  Serial.print("Time: "); Serial.println(timeStamp);
  //Create Date
  dateStamp = Day + "-" + Month + "-" + Year;
  Serial.print("Date: "); Serial.println(dateStamp);
  //Weekday
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
  Serial.print("Day: "); Serial.println(dayStamp);
}
/* ________________________________________Setup________________________________________ */
void setup()
{
  Serial.begin(9600);
  connectToWifi();
  timeClient.begin();
}
/* ________________________________________Loop_________________________________________ */
void loop() {
  //Printing Time
  timeClient.update();
  timeNow = timeClient.getEpochTime() + (19800); //Set offset wrt your Timezone - 5 hours 30 minutes = 19800s (India is GMT+5:30)
  getDetailedTimeStamp(timeNow); 
  Serial.println();
  delay(1000);
}
