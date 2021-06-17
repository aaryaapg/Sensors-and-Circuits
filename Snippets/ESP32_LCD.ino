 /*
  * ______________________________________Connections______________________________________ 
  * RFID RC522    ESP32
  * RST  -------> 12         
  * CE   -------> 13
  * DC   -------> 14
  * Din  -------> 15 
  * CLK  -------> 2
  * Vcc  -------> 3.3V
  * BL   -------> 3.3V
  * GND  -------> GND
  */
 
 /* ______________________________________Libraries______________________________________ */
 //LCD
 #include <Adafruit_PCD8544.h> //Library for Nokia 5110 LCD
 #include <Adafruit_GFX.h> //Adafruit Graphics Library
 #include <gfxfont.h>
 #include <SPI.h> //For communication between LCD and ESP32

 /* ______________________________________Macros______________________________________ */
 // Nokia 5110 LCD module connections
 #define CLK 2
 #define DIN 15
 #define DC 14
 #define CE 13
 #define RST 12

 /* ______________________________________Declarations and Variables______________________________________ */
 Adafruit_PCD8544 display = Adafruit_PCD8544(CLK, DIN, DC, CE, RST); 

 /* ______________________________________Setup______________________________________ */
  void setup() {
    Serial.begin(9600);
    display.begin(); //Initialize the display
    display.clearDisplay();
    display.setContrast(50); // Adjust for your display
    display.setTextColor(BLACK, WHITE);
    display.display();
  }

 /* ______________________________________Loop______________________________________ */
 void loop() {
  display.setCursor(12,12);
  display.println("Hello!"); //LCD
  display.display();
 }
