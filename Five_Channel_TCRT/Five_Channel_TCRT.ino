/* ______________________________________Connections______________________________________ 
5 Channel    NodeMCU
OUT1  -------> D4         
OUT2  -------> D5
OUT3  -------> D7
OUT4  -------> D6
OUT5  -------> NC
GND   -------> GND
VCC   -------> 3.3V
*/

/* ______________________________________Macros______________________________________ */
#define N 10 //Total Number of weight slabs
#define w 5 //Weight of each slab
//Note: White = 1, Black = 0

/* ______________________________________Setup______________________________________ */
void setup() {
  Serial.begin(9600);
  pinMode(D1,INPUT);
  pinMode(D2,INPUT);
  pinMode(D3,INPUT);
  pinMode(D4,INPUT);
  pinMode(D5,INPUT);  
}

/* ______________________________________Loop______________________________________ */
void loop() {
  int Bin0 = digitalRead(D1);
  int Bin1 = digitalRead(D2);
  int Bin2 = digitalRead(D3);
  int Bin3 = digitalRead(D4);
  int Bin4 = digitalRead(D5);
  int Position = ToDecimal(Bin0, Bin1, Bin2, Bin3, Bin4); //Position of the pin is the decimal equivalent of the received values
  int Weight_Lifted = (N - Position)*w;
  /*Formula
  * Weight Lifted = (Total Number of Slabs - Position of Pin)*(Weight of each Slab)
  */
  Serial.print("Digital Value = ");
  Serial.print(Bin4);
  Serial.print(Bin3);
  Serial.print(Bin2);
  Serial.print(Bin1);
  Serial.println(Bin0);
  Serial.print("Position = "); Serial.println(Position);
  Serial.print("Weight Lifted = "); Serial.println(Weight_Lifted);
  
  delay(500); // delay in between reads for stability
}


int ToDecimal(int x0,int x1, int x2, int x3, int x4)
{
  int Sum = 0;
  if (x0)
    Sum += 1;
  if (x1)
    Sum += 2;
  if (x2)
    Sum += 4;
  if (x3)
    Sum += 8;
  if (x4)
    Sum += 16;
  return(Sum);
}
