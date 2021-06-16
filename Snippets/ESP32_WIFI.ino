/* ______________________________________Libraries______________________________________ */
//WiFi
#include <WiFi.h>

/* ______________________________________Macros______________________________________ */
//Wifi
const char* ssid     = "********";
const char* password = "********";

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
    
}
