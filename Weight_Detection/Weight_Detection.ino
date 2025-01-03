/*Connections
 * 5 Channel TCRT ----> Arduino Uno
 *    OUT1        ---->   A0 (LSB)
 *    OUT2        ---->   A1
 *    OUT3        ---->   A2
 *    OUT4        ---->   A3
 *    OUT5        ---->   A4 (MSB)
 */
#define N 10 //Total Number of weight slabs
#define w 5 //Weight of each slab
//Note: White = 1, Black = 0
void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  
}

void loop() {
  int Bin0 = digitalRead(A0);
  int Bin1 = digitalRead(A1);
  int Bin2 = digitalRead(A2);
  int Bin3 = digitalRead(A3);
  int Bin4 = digitalRead(A4);
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
