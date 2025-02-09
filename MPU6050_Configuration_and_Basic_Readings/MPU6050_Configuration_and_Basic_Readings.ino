/*
   Connections
   MPU6050 --- ESP32
   VCC ------- 3.3V
   GND ------- GND
   SCL ------- 18
   SDA ------- 19
   XDA ------- NC
   XCL ------- NC
   AD0 ------- GND
   INT ------- NC
*/

/* ______________________________________Libraries______________________________________ */
//Include Wire Library for I2C
#include <Wire.h>
/* ______________________________________Macros______________________________________ */
//Define I2C Pins for MPU6050
#define SCL 18
#define SDA 19
//Define sensitivity scale factors for accelerometer and gyro based on the GYRO_CONFIG and ACCEL_CONFIG register values
#define aScaleFactor 16384
#define gScaleFactor 131

/* ______________________________________Variables and Declarations______________________________________ */
//Since pin AD0 is Grounded, address of the device is b1101000 (0x68) [Check Sec 9.2 of Datasheet]
const uint8_t MPU6050SlaveAddress = 0x68;

//Few configuration register addresses
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

int16_t AX_raw, AY_raw, AZ_raw, GX_raw, GY_raw, GZ_raw, Temp_raw;
int16_t GX_off, GY_off, GZ_off;
double Ax, Ay, Az, T, Gx, Gy, Gz;
double A_Roll, A_Pitch, Gradient, Mag;

/* ______________________________________Setup______________________________________ */
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  MPU6050_Init(); //Setup the MPU6050 registers
  MPU6050_Cali(); //Calibrate it by taking 1000 readings and calculating their average to get the offsets
}

/* ______________________________________Loop______________________________________ */
void loop() {
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  //Subtract the offsets from the raw data
  GX_raw = GX_raw - GX_off;
  GY_raw = GY_raw - GY_off;
  GZ_raw = GZ_raw - GZ_off;
  //Divide each with their sensitivity scale factor
  Ax = (double)AX_raw / aScaleFactor;
  Ay = (double)AY_raw / aScaleFactor;
  Az = (double)AZ_raw / aScaleFactor;
  T = (double)Temp_raw / 340 + 36.53; //temperature formula from datasheet
  Gx = (double)GX_raw / gScaleFactor;
  Gy = (double)GY_raw / gScaleFactor;
  Gz = (double)GZ_raw / gScaleFactor;
  Mag = sqrt(Ax * Ax + Ay * Ay + Az * Az);
  A_Roll = (atan2(Ay, Az)) * 57.29577951;
  Gradient = tan(atan2(Ay, Az)) * 100;
  A_Pitch = (atan2(Ax, Az)) * 57.29577951;
  

  //  Serial.print("Ax: "); Serial.print(Ax);
  //  Serial.print(" Ay: "); Serial.print(Ay);
  //  Serial.print(" Az: "); Serial.print(Az);
  //  Serial.print(" T: "); Serial.print(T);
  //  Serial.print(" Gx: "); Serial.print(Gx);
  //  Serial.print(" Gy: "); Serial.print(Gy);
  //  Serial.print(" Gz: "); Serial.println(Gz);
  Serial.print(" A_Roll: "); Serial.println(A_Roll);
  //  Serial.print(" Gradient: "); Serial.println(Gradient);
  Serial.print(" A_Pitch: "); Serial.println(A_Pitch);
  //  Serial.print(" Magnitude: "); Serial.println(Mag*1000);

  delay(1000);

}

/* ______________________________________Functions______________________________________ */
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

//A function that lets us write data to the slave's registers easily
void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data) {
  Wire.beginTransmission(deviceAddress); //Get the slave's attention, tell it we're sending a command byte. Slave = MPU6050
  Wire.write(regAddress); //The command byte sets pointer to register whose address is given
  Wire.write(data); //Now that the pointer is ‘pointing’ at the specific register you wanted, this command will replace the byte stored in that register with the given data.
  Wire.endTransmission(); //This tells the slave that you’re done giving it instructions for now
}

//Calibrate MPU6050 to get offsets
void MPU6050_Cali() {
  Serial.println("Calibration in progress");
  for (int i = 0; i < 1000; i++) {
    Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
    GX_off += GX_raw;
    GY_off += GY_raw;
    GZ_off += GZ_raw;
  }
  //Take average
  GX_off = GX_off / 1000;
  GY_off = GY_off / 1000;
  GZ_off = GZ_off / 1000;
  Serial.println("Calibration Done");
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
