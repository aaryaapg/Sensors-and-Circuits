void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int A_Value = analogRead(A0);
  int D_Value = digitalRead(D0);
  //Range 200 to 950, Approx 1 inch
  // print out the value you read:
  Serial.print("Analog Value = ");
  Serial.print(A_Value);
  Serial.print(", Digital Value = ");
  Serial.print(D_Value);
  if(D_Value)
    Serial.println(", Black");
  else 
    Serial.println(", White");
  delay(100);        // delay in between reads for stability
}
