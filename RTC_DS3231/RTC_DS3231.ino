/*
Connections
RFID RC522    ESP32
SDA  -------> 15         
SCK  -------> 18
MOSI -------> 23
MISO -------> 19
IRQ  -------> NC
GND  -------> GND
RST  -------> 27
VCC  -------> 3.3V
Pushbutton -> 22
RGB LED      ESP32
Red   ------> 13
Anode ------> 3.3V
Green ------> 12
Blue  ------> 14
MPU6050      ESP32
VCC --------> 3.3V         
GND --------> GND 
SCL --------> 26
SDA --------> 25
XDA --------> NC
XCL --------> NC
AD0 --------> GND
INT --------> NC
RTCDS3231    ESP32
32K --------> NC
SQW --------> NC
SCL --------> 22
SDA --------> 21
VCC --------> 3.3V
GND --------> GND
-> https://github.com/espressif/arduino-esp32/issues/595#issuecomment-413829444
-> https://gist.github.com/SaheblalBagwan/5e538a727a5f1b0c129da36786d67159
*/

/* ______________________________________Libraries______________________________________ */
#include <Arduino.h>
//RFID
#include <SPI.h>
#include<MFRC522.h>
//WD
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
//MPU6050
#include <Wire.h> //For communication between MPU6050 and ESP32 (I2C)
#include <Math.h>
//RTC 
#include <RTClib.h>
//ESP-NOW and ESP
#include <esp_now.h>
#include <WiFi.h>

/* _______________________________________Macros________________________________________ */
//RFID
#define SS_PIN 15
#define RST_PIN 27
//Button
#define buttonPin 33
//For RGB LEDs
#define RED 13
#define GREEN 12
#define BLUE 14
//For MPU6050 - threshod for number of rotations
#define Upper_Threshold 0.85
//MPU6050 I2C Pins
#define SCL 26
#define SDA 25
//Define sensitivity scale factors for accelerometer and gyro based on the GYRO_CONFIG and ACCEL_CONFIG register values
#define aScaleFactor 16384
#define gScaleFactor 131
//ESP-NOW
uint8_t broadcastAddress[] = {0xAC, 0x67, 0xB2, 0x3C, 0x91, 0xCC}; //RECEIVER MAC Address
/* _____________________________Declarations_and_Variables______________________________ */
//Struct message to send
typedef struct struct_cycle {
  String RFIDCode;
  float duration;
  int count;
} struct_cycle;
struct_cycle c1;
//RFID
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
byte uid[4]; // Init array that will store UID
//String RFIDCode = "";
//Button
int buttonState; //Logout will be initiated if button state is HIGH
//Tasks
TaskHandle_t taskRFID_handle = NULL;
TaskHandle_t taskReadSensors_handle = NULL;
TaskHandle_t taskLogoutButton_handle = NULL;
TaskHandle_t taskLogoutProcedure_handle = NULL;
//MPU6050: Since pin AD0 is Grounded, address of the device is b1101000 (0x68) [Check Sec 9.2 of Datasheet]
const uint8_t MPU6050SlaveAddress = 0x68;
//MPU6050: Few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B; //Register 59 (14 Registers (59 to 72) contain accel, temp and gyro data)
//Accelerometer and Gyroscope Variabels
int16_t AX_raw, AY_raw,AZ_raw, GX_raw, GY_raw, GZ_raw, Temp_raw;
double Ax, Ay, Az, T, Gx, Gy, Gz;
//Counter Variables for Accelerometer Rotations
int state; //State Variable
int c_state = 0; //current state
int p_state = 0; //previous state
int count = 0; //counting all edges
//int realcount; //Count for just low to high edges
//duration
long int elapsedTime = 0, loginTime = 0;
//float duration;
//path
String path = "";

/* ______________________________________Functions______________________________________ */
//Function to get RGB colours
void RGB_color(bool r, bool g, bool b){
  digitalWrite(RED, r);
  digitalWrite(GREEN, g);
  digitalWrite(BLUE, b);
  delay(1000);
}
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
//A function that lets us write data to the slave's registers easily
void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data) {
  Wire.beginTransmission(deviceAddress); //Get the slave's attention, tell it we're sending a command byte. Slave = MPU6050
  Wire.write(regAddress); //The command byte sets pointer to register whose address is given
  Wire.write(data); //Now that the pointer is ‘pointing’ at the specific register you wanted, this command will replace the byte stored in that register with the given data.
  Wire.endTransmission(); //This tells the slave that you’re done giving it instructions for now
}
//Configure and setup MPU6050 Registers
void MPU6050_Init() {
  delay(150);
  //Step 1: Set sample rate divider to get the desired sampling rate of 1kHz based on the formula given in datasheet
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  //Step 2: Set PLL with X axis gyroscope as the clock reference for improved stability.
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  //Step 3: This functionality is not required. Disable it.
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  //Step 4: Disable external Frame Synchronization and disable DLPF so that Gyroscope Output Rate = 8kHz
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  //Step 5: Set gyroscope full range to +- 250 dps, so that the gyroscope sensitivity scale factor is 131
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);
  //Step 6: Set accelerometer full range to +- 2g, so that the accelerometer sensitivity scale factor is 16384
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);
  //Step 7: This functionality is not required. Disable it.
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  //Step 8: Enable the Data Ready interrupt
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  //Step 9: Do not reset signal paths
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  //Step 10: The functionalities provided by the bits of this register are not requires now. Disable them
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}
//14 Registers (59 to 72) contain accel, temp and gyro data. We need to access it
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress) {
  Wire.beginTransmission(deviceAddress); //Get the slave's attention, tell it we're sending a command byte. Slave = MPU6050
  Wire.write(regAddress); //The command byte sets pointer to register whose address is given
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14); //Used by the master to request bytes from a slave device
  AX_raw = (((int16_t)Wire.read() << 8) | Wire.read());
  AY_raw = (((int16_t)Wire.read() << 8) | Wire.read());
  AZ_raw = (((int16_t)Wire.read() << 8) | Wire.read());
  Temp_raw = (((int16_t)Wire.read() << 8) | Wire.read());
  GX_raw = (((int16_t)Wire.read() << 8) | Wire.read());
  GY_raw = (((int16_t)Wire.read() << 8) | Wire.read());
  GZ_raw = (((int16_t)Wire.read() << 8) | Wire.read());
}
/* ________________________________________Tasks________________________________________ */
void taskLogoutProcedure(void * parameter){
  //Execute Logout procedure
  Serial.println("Executing logout procedure");
  elapsedTime = millis() - loginTime;
  c1.duration = elapsedTime/1000;
  Serial.print("Duration: "); Serial.print(c1.duration); Serial.println(" Seconds");
  //Send data
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &c1, sizeof(c1));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  //Put it into deep sleep
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1); //1 = High, 0 = Low
  Serial.println("Going to sleep now");
  delay(1000);
  esp_deep_sleep_start();
}
void taskLogoutButton(void * parameter){
  for(;;){
    buttonState = digitalRead(buttonPin);
    if(buttonState==HIGH){
      //Suspend taskReadSensors
      Serial.println("Suspending ReadSensors Task");
      if(taskReadSensors_handle != NULL) {
        vTaskSuspend(taskReadSensors_handle);
      }
      //Create tastLogoutProcedure
      Serial.println("Creating LogoutProcedure Task");
      xTaskCreate(
        taskLogoutProcedure,          /* Task function. */
        "taskLogoutProcedure",        /* String with name of task. */
        10000,            /* Stack size in bytes. */
        NULL,             /* Parameter passed as input of the task */
        1,                /* Priority of the task. */
        &taskLogoutProcedure_handle);            /* Task handle. */
      //Suspend own task
      Serial.println("Suspending LogoutButton Task");
      vTaskSuspend( NULL );
    }
  }
}
void taskReadSensors(void * parameter){ 
  delay(1000);
  Serial.println("Reading Sensors");
  for(;;){
    //Read sensors here
    Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
    //Divide each with their sensitivity scale factor
    Ax = (double)AX_raw / aScaleFactor;
    Ay = (double)AY_raw / aScaleFactor;
    Az = (double)AZ_raw / aScaleFactor;
    T = (double)Temp_raw / 340 + 36.53; //temperature formula from datasheet
    Gx = (double)GX_raw / gScaleFactor;
    Gy = (double)GY_raw / gScaleFactor;
    Gz = (double)GZ_raw / gScaleFactor; 
    //Comparator
    if(Az > Upper_Threshold) {
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
    }
    else{
      count = count;
    }
    // save the current state as the last state, for next time through the loop
    p_state = c_state;
    c1.count = count / 2;
    Serial.print(" State: "); Serial.print(state);
    Serial.print(" Count: "); Serial.println(c1.count);
    delay(500);
  }
}
void taskRFID( void * parameter ){
  for(;;){
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    //Check for an RFID Card
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
      for (byte i = 0; i < 4; i++) {
        uid[i] = rfid.uid.uidByte[i];
      }
      //Get RFID card
      for (byte j = 0; j < rfid.uid.size; j++) 
      {
        c1.RFIDCode.concat(String(rfid.uid.uidByte[j] < 0x10 ? "0" : ""));
        c1.RFIDCode.concat(String(rfid.uid.uidByte[j], HEX));
      }
      //Print RFID Tag
      c1.RFIDCode.toUpperCase();
      c1.RFIDCode=String(c1.RFIDCode);
      Serial.print("RFID Code: "); Serial.println(c1.RFIDCode);
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      //for duration
      loginTime = millis();
      //Create document path
      //path = "gyms/addiction/users/" + RFIDCode + "/activity/" + dateStamp + "/today/cycle";
      //Serial.print("Path: "); Serial.println(path);
      //Turn Antenna off
      rfid.PCD_AntennaOff();  
      Serial.println("Antenna Off "); //Successfully logged in
      RGB_color(0, 1, 1); //RED - occupied
      xTaskCreate(
        taskReadSensors,          /* Task function. */
        "taskReadSensors",        /* String with name of task. */
        10000,            /* Stack size in bytes. */
        NULL,             /* Parameter passed as input of the task */
        1,                /* Priority of the task. */
        &taskReadSensors_handle);            /* Task handle. */
      xTaskCreate(
        taskLogoutButton,          /* Task function. */
        "taskLogoutButton",        /* String with name of task. */
        10000,            /* Stack size in bytes. */
        NULL,             /* Parameter passed as input of the task */
        1,                /* Priority of the task. */
        &taskLogoutButton_handle);            /* Task handle. */
      Serial.println("Suspending RFID Task");
      vTaskSuspend( NULL );
    }
    else{
      Serial.println("Please Scan");
    }
  }
}

/* ________________________________________Setup________________________________________ */
void setup() {
  Serial.begin(112500);
  //print_wakeup_reason();
  //RFID
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  //ESP-NOW 
  WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station  
  //ESP-NOW: Init ESP-NOW
  if (esp_now_init() != ESP_OK) { 
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  //ESP-NOW: Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);  
  //ESP-NOW: Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;  
  //ESP-NOW: Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  //Setup MPU6050
  Wire.begin(SDA, SCL);
  MPU6050_Init(); //Setup the MPU6050 registers
  //RGB LED
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(buttonPin, INPUT);  
  digitalWrite(RED,HIGH); //Note: Common Anode LEDs work on active LOW signals
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);
  RGB_color(1, 0, 1); //Green - unoccupied at the start  
  delay(1000);
  xTaskCreate(
    taskRFID,          /* Task function. */
    "taskRFID",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    &taskRFID_handle);            /* Task handle. */
}
/* ________________________________________Loop_________________________________________ */
void loop() {
  delay(1000);
}
