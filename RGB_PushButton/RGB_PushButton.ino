/*
 * ______________________________________About______________________________________ 
 * Each button press gives a different colour. 
 * There are 7 colours. 
 * 8th button press will turn the LED OFF. 
 * The colours will repeat with subsequent button presses.
 * uC = ESP32
 * ______________________________________Connections______________________________________ 
 * RGB LED          ESP32
 * Red     -------> 12
 * Anode   -------> 3.3V
 * Green   -------> 13
 * Blue    -------> 14
 * 
 * Button  -------> 15 (via 120 Ohm R) of ESP32
 */

/* ______________________________________Macros______________________________________ */
//For RGB LEDs
#define RED 12
#define GREEN 13
#define BLUE 14
//For Push Button
#define buttonPin 15

/* ______________________________________Variables and Declarations______________________________________ */
int buttonState;
int count = 0; //Colour of the LED will change wrt the number of times the button was pressed, hence we need a count variable.

/* ______________________________________Setup______________________________________ */
void setup() {
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(buttonPin, INPUT);
  //Note: Common Anode LEDs work on active LOW signals
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);
}

/* ______________________________________Loop______________________________________ */
void loop() { 
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW){
    count = count + 1;
    switch(count){
      case 1: {
        RGB_color(0, 0, 0); //White
        break;
      }
      case 2: {
        RGB_color(0, 0, 1); //Yellow
        break;
      }
      case 3: {
        RGB_color(0, 1, 0); //Magenta
        break;
      }
      case 4: {
        RGB_color(0, 1, 1); //RED
        break;
      }
      case 5: {
        RGB_color(1, 0, 0); //Bright Blue or Cyan
        break;
      }
      case 6: {
        RGB_color(1, 0, 1); //Green
        break;
      }
      case 7: {
        RGB_color(1, 1, 0); //Blue
        break;
      }
      default: {
        RGB_color(1, 1, 1); 
        count = 0;
        break;//LED OFF when count = 0
      }
    }
  }
  else {
    count = count;
  }   
  Serial.println(count);
}
/* ______________________________________Functions______________________________________ */
void RGB_color(bool r, bool g, bool b)
{
  digitalWrite(RED, r);
  digitalWrite(GREEN, g);
  digitalWrite(BLUE, b);
  delay(1000);
}
