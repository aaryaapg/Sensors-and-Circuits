/*
Connections
DHT11   Arduino Uno
VCC ---> 5V
Data --> 12
GND ---> GND
*/

/* ______________________________________Libraries______________________________________ */
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>      // DHT11 temperature and humidity sensor Library

/* _______________________________________Macros________________________________________ */
#define DHTPIN 12
#define DHTTYPE DHT11

/* _____________________________Declarations_and_Variables______________________________ */
DHT dht = DHT(DHTPIN, DHTTYPE);   // DHT dht = DHT(DHTPIN,Sensor_type), "dht" is the name of the sensor (variable)
int h, tc;
float tf, hif, hic;

/* ________________________________________Setup________________________________________ */
void setup() {
  Serial.begin(9600);   //Begin serial communication at a baud rate of 9600
  dht.begin();          //Initialize the sensor
  Serial.println("Humidity and temperature\n\n");
  delay(100);
}
/* ________________________________________Loop_________________________________________ */
void loop() {
  h = dht.readHumidity();
  tc = dht.readTemperature();   // Get Temperature in C
  tf = dht.readTemperature(true);   // Get Temperature in F   
  hif = dht.computeHeatIndex(tf, h); // Heat index in F, Default
  hic = dht.computeHeatIndex(tc, h, false);  // Heat index in C
  
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.print("% |");
  Serial.print("Temperature = ");
  Serial.print(tc); 
  Serial.print("\xC2\xB0 C ie, ");    // Serial.print(" \xC2\xB0"); is used to print the degree symbol in the serial monitor
  Serial.print(tf);
  Serial.print("\xC2\xB0 F |");
  Serial.print("Heat index = ");
  Serial.print(hic); 
  Serial.print("\xC2\xB0 C ie, ");
  Serial.print(hif);
  Serial.println("\xC2\xB0 F |");
}
