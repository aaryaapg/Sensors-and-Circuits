#include<math.h>
#define Zpin A0 //Accelerometer Z axis output will be given to A0
int Zraw;
double Zg;
int state;
int c_state = 0; //current state
int p_state = 0; //previous state
int count = 0; //counting all edges
int realcount; //Count for just low to high edges

void setup() {
  Serial.begin(9600);
}
void loop() {
  Zraw = analogRead(Zpin);
  Zg = ( ( ( (double)(Zraw * 3.3)/1024) - 1.65 ) / 0.30 ); // Acceleration in z-direction in g units 
  Serial.println(Zraw);
 
  if(Zg > 0.9) {
    state = 1;
  }
  else {
    state = 0;
  }
  c_state = state;
  // compare the current state to its previous state
  if (c_state != p_state) {
      // if the state has changed, increment the counter
      count++;
      // Delay a little bit to avoid bouncing
      delay(50);
  }
  else{
    count = count;
  }
  // save the current state as the last state, for next time through the loop
  p_state = c_state;
  //Serial.print(Zraw);
  //Serial.print(" | ");
  Serial.print(state);
  Serial.print(" | Count = ");
  realcount = count / 2;
  Serial.println(realcount);
        delay(500);

  /* 
   *  The problem here is:
   *  ADXL335 is extremely sensitive to touch or any kind of shock. The values keep fluctuating even when the device is kept steady. I cannot see this device being used in a gym. 
  */
}
  
