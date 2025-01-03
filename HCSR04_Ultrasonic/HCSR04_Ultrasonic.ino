/*
Connections
Name          Arduino Uno   ESP32   ESP8266   Pro Mini
---> 
---> 
---> 
*/

/* ______________________________________Libraries______________________________________ */
#include <Arduino.h>
/* _______________________________________Macros________________________________________ */
#define TRIGGER D6
#define ECHO D5
#define soundVelocity 0.034
/* _____________________________Declarations_and_Variables______________________________ */
/* ______________________________________Functions______________________________________ */
/* ________________________________________Setup________________________________________ */
void setup() {
  Serial.begin (115200);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
}
/* ________________________________________Loop_________________________________________ */
void loop() {
  long duration, distance;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  digitalWrite(TRIGGER, LOW);
  
  duration = pulseIn(ECHO, HIGH);
  distance= duration*(soundVelocity/2);
  Serial.print(distance); Serial.println(" cm");
  delay(500);
}


// NodeMCU Pin D1 > TRIGGER | Pin D2 > ECHO
