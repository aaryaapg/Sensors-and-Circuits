const int Input = A0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  int Raw = analogRead(Input);
  Serial.print(Raw);
  int Val = map(Raw, 410, 580, -100, 100);
  float Ing = (float)Val/(100.00);
  Serial.print("\t"); 
  Serial.print(Ing);
  Serial.println("g   ");
  delay(100);
}
