
#include<math.h>
#define Xpin A0 //Accelerometer X axis output will be given to A0
#define Ypin A1 //Accelerometer Y axis output will be given to A1
#define Zpin A2 //Accelerometer Z axis output will be given to A2

int Xraw,Yraw,Zraw; //Raw Analog values given by pin A0,A1 and A2
double Xg,Yg,Zg;
double theta, psi, phi;
double roll, pitch;
double Accn;
void setup() {
  Serial.begin(9600);
}

void loop() {
  Xraw = analogRead(Xpin);
  Yraw = analogRead(Ypin);
  Zraw = analogRead(Zpin);
  /*
  Accel. in g ={((V_raw * Vs)/1024)-(Vs/2)}/sensitivity 
  Vs is the reference voltage, here: 3.3V
  1024 is the resolution of the ADC present on Arduino pro mini - 10 bit ADC
  */
  Xg = ( ( ( (double)(Xraw * 3.3)/1024) - 1.65 ) / 0.30 ); // Acceleration in x-direction in g units
  Yg = ( ( ( (double)(Yraw * 3.3)/1024) - 1.65 ) / 0.30 ); // Acceleration in y-direction in g units 
  Zg = ( ( ( (double)(Zraw * 3.3)/1024) - 1.65 ) / 0.30 ); // Acceleration in z-direction in g units  
  
  theta = (atan(-1 * Xg/sqrt(sq(Yg)+sq(Zg))))*(180/3.14); //Angle of inclination of X Axis
  psi = (atan(Yg/sqrt(sq(Xg)+sq(Zg))))*(180/3.14);   //Angle of inclination of Y Axis
  phi = (atan(sqrt(sq(Xg)+sq(Yg))/Zg))*(180/3.14);   //Angle of inclination of Z Axis
  
  //roll = ( ( (atan2(Yg,Zg) * 180) / 3.14 ) + 180 ); //Formula for roll - NO
  //pitch = ( ( (atan2(Zg,Xg) * 180) / 3.14 ) + 180 ); //Formula for pitch - NO
  
  Accn = sqrt(sq(Xg) + sq(Yg) + sq(Zg)); //Magnitude of the Acceleration vector
 
  Serial.print(Xg);
  Serial.print(" g, ");
  Serial.print(Yg);
  Serial.print(" g, ");
  Serial.print(Zg);
  Serial.print(" g | ");
  Serial.print(theta);
  Serial.print(", ");
  Serial.print(psi);
  Serial.print(", ");
  Serial.print(phi);
  //Serial.print(" | ");
  //Serial.print(roll);
  //Serial.print(", ");
  //Serial.print(pitch);
  Serial.print(" | ");
  Serial.print(Accn);
  Serial.println(" g");

}
