/*
 * ______________________________________About______________________________________ 
 * This sketch displays the current Date, Day and Time on the Serial Monitor.
 * uC = ESP8266 or ESP32
 */

/* ______________________________________Libraries______________________________________ */
//WiFi
#include <WiFi.h>
//NTP
#include <TimeLib.h> 
#include <NTPClient.h>
#include <WiFiUdp.h>

/* ______________________________________Macros______________________________________ */
//Wifi
const char* ssid     = "Padhyegurjar";
const char* password = "mncb8tbbt8";

/* ______________________________________Declarations and Variables______________________________________ */
//NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP); 
long int timeNow = 0;
String timeStamp, dateStamp, dayStamp;

/* ______________________________________Setup______________________________________ */
void setup()
{
  Serial.begin(9600);
  // We start by connecting to a WiFi network
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
}

/* ______________________________________Loop______________________________________ */
void loop()
{
  timeClient.update();
  //Set offset wrt your Timezone - 5 hours 30 minutes = 19800s (India is GMT+5:30)
  timeNow = timeClient.getEpochTime() + (19800);
  getDetailedTimeStamp(timeNow);
  Serial.print("Date: "); Serial.println(dateStamp);
  Serial.print("Day: "); Serial.println(dayStamp);
  Serial.print("Time: "); Serial.println(timeStamp);
}

/* ______________________________________Functions______________________________________ */
//Function to convert epoch time into a readable format
void getDetailedTimeStamp(int logs) {
  timeStamp = ((hour(logs)<10)?("0"+String(hour(logs))):(String(hour(logs)))) + ":" + ((minute(logs)<10)?("0"+String(minute(logs))):(String(minute(logs)))) + ":" + ((second(logs)<10)?("0"+String(second(logs))):(String(second(logs))));
  dateStamp = ((day(logs)<10)?("0"+String(day(logs))):(String(day(logs))))+ "-" + ((month(logs)<10)?("0"+String(month(logs))):(String(month(logs)))) + "-" + String(year(logs));
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
