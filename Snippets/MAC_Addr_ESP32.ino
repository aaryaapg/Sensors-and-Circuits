//Getting MAC address of ESP32. No need to connect to WiFi for this, just include the WiFi library.

/* ______________________________________Libraries______________________________________ */
#include <Arduino.h>
//Wifi
#include <WiFi.h>
/* _______________________________________Macros________________________________________ */
//Wifi
const char* ssid     = "ssid";
const char* password = "pw";
/* _____________________________Declarations_and_Variables______________________________ */
/* ______________________________________Functions______________________________________ */
/* ________________________________________Setup________________________________________ */
void setup()
{
  Serial.begin(9600);
  //Connecting to a WiFi network
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
  //Getting MAC Address of ESP32
  WiFi.mode(WIFI_MODE_STA); //the ESP32 connects to an access point, which is the router
  Serial.print("MAC address: "); Serial.println(WiFi.macAddress());
}
/* ________________________________________Loop_________________________________________ */
void loop() {
  // put your main code here, to run repeatedly:
}
