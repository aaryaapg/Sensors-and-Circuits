#include <SPC.h>
int fsrRaw;
int Sq;
int Count;

SPC spc;
void setup() {
  Serial.begin(9600);   
}

void loop() {
  fsrRaw = analogRead(A0);
  Sq = spc.SPC_getSquare(200, fsrRaw);
  Count = spc.SPC_getCount(Sq);
  Serial.print(" | Square w/f: "); Serial.print(Sq);
  Serial.print(" | Count: "); Serial.println(Count);
  //Serial.print(" | Raw Value: "); Serial.print(fsrRaw);
  //Serial.print(" | Threshold: "); Serial.println(200);
}
