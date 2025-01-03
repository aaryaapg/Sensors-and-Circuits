#define fsrPin A0
#define VCC 3300 //in mV (Because we are using NodeMCU here)
#define R 10000 //10 KOhm resistor
#define threshold 200 //Threshold value w.r.t sensor 
int fsrRaw; //Raw analog reading - being used as CURRENT STATE of the sensor
int fsrVoltage; //in mV
long fsrResistance;
long Force;
int Sq; //Also being used as CURRENT STATE of the square wave
int Count;
/*List of Variables for functions*/
int LAST_SENSOR_STATE = LOW;   //sensor's previous state
int SQUARE_STATE = 0; //Converting analog waveform into square waveform w.r.t threshold
int LAST_SQUARE_STATE = LOW;
int EDGE_COUNT = 0;
int POSEDGE_COUNT = 0;
void setup() {
  Serial.begin(9600);   
}

void loop() {
  fsrRaw = analogRead(fsrPin);
  fsrVoltage = map(fsrRaw, 0, 1023, 0, VCC); // analog voltage reading ranges from about 0 to 1023 which maps to 0V to 3.3V (= 3300mV)
  if (fsrVoltage == 0) {
    Serial.println("No pressure");  
  } 
  else {
    // The voltage = Vcc * R / (R + FSR) where R = 10K and Vcc = 3.3V
    // so FSR = ((Vcc - V) * R) / V   
    fsrResistance = ((VCC - fsrVoltage)*(R))/ fsrVoltage;
  }
  Sq = SPC_getSquare(threshold, fsrRaw);
  Count = SPC_getCount(Sq);
  //Display on Serial Monitor
  //Serial.print("Raw Value: "); Serial.print(fsrRaw);
  //Serial.print(" | Voltage Equivalent: "); Serial.print(fsrVoltage); Serial.print(" mV");
  //Serial.print(" | Square Value: "); Serial.println(squareState);
  //Serial.print(" | Resistance: "); Serial.print(fsrResistance); Serial.println(" Ohm");
  //Serial.print(" | Force: "); Serial.print(Force); Serial.println(" N");
  Serial.print(Sq); Serial.print(" | "); Serial.println(Count);
  if (Sq) {
    Serial.println("Occupied");
  }
  else{
    Serial.println("Not Occupied");
  }
}

int SPC_getSquare(int THRESHOLD, int CURRENT_SENSOR_STATE){
  if (CURRENT_SENSOR_STATE >= THRESHOLD) { //if it's above the threshold
     if (LAST_SENSOR_STATE < THRESHOLD) { // check that the previous value was below the threshold
        //Serial.println("The sensor value just went above the threshold"); 
        SQUARE_STATE = 1;
     }
  }
  else { //if it's below the threshold
    if (LAST_SENSOR_STATE > THRESHOLD) { // check that the previous value was above the threshold
      //Serial.println("The sensor value just went below the threshold"); 
      SQUARE_STATE = 0;
    }
  }
  LAST_SENSOR_STATE = CURRENT_SENSOR_STATE; // save the sensor state for next comparison
  return SQUARE_STATE;
}

int SPC_getCount(int CURRENT_SQUARE_STATE){
  if(CURRENT_SQUARE_STATE != LAST_SQUARE_STATE){
    EDGE_COUNT++;
  }
  else {
    EDGE_COUNT = EDGE_COUNT;
  }
  LAST_SQUARE_STATE = CURRENT_SQUARE_STATE;
  POSEDGE_COUNT = EDGE_COUNT/2;
  return POSEDGE_COUNT;
}
