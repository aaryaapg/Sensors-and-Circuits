/*
 * Connections
 * MPU6050 --- ESP8266
 * VCC ------- 3.3V         
 * GND ------- GND 
 * SCL ------- D6
 * SDA ------- D7
 * XDA ------- NC
 * XCL ------- NC
 * AD0 ------- NC
 * INT ------- NC
 */

#include <Wire.h>

// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D1;
const uint8_t sda = D2;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AX_raw, AY_raw,AZ_raw, GX_raw, GY_raw, GZ_raw, Temp_raw;
double Ax, Ay, Az, T, Gx, Gy, Gz, A_Roll, A_Pitch, Gradient, Mag;
void setup() {
  Serial.begin(9600);
  Wire.begin(sda, scl);
  MPU6050_Init();
}

void loop() {  

  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  //divide each with their sensitivity scale factor
  Ax = (double)AX_raw/AccelScaleFactor;
  Ay = (double)AY_raw/AccelScaleFactor;
  Az = (double)AZ_raw/AccelScaleFactor;
  T = (double)Temp_raw/340+36.53; //temperature formula
  Gx = (double)GX_raw/GyroScaleFactor;
  Gy = (double)GY_raw/GyroScaleFactor;
  Gz = (double)GZ_raw/GyroScaleFactor;
  A_Roll = (atan2(Ay, Az)) *57.29577951;
  Gradient = tan(atan2(Ay, Az)) * 100;
  Mag = sqrt(Ax * Ax + Ay * Ay + Az * Az);
  //A_Pitch = (atan2(Az, Ax)) *57.29577951; - NO
  // float pitch = atan2(-ax, sqrt(ay * ay + az * az)); - formula https://stackoverflow.com/questions/55866808/attitude-estimation-from-accelerometer-and-gyroscope-of-an-imu
 
  
  //Serial.print("Ax: "); Serial.print(Ax);
  //Serial.print(" Ay: "); Serial.print(Ay);
  //Serial.print(" Az: "); Serial.print(Az);
  //Serial.print(" T: "); Serial.print(T);
  //Serial.print(" Gx: "); Serial.print(Gx);
  //Serial.print(" Gy: "); Serial.print(Gy);
  //Serial.print(" Gz: "); Serial.print(Gz);
  Serial.print(" A_Roll: "); Serial.print(A_Roll);
  Serial.print(" Gradient: "); Serial.println(Gradient);
  //Serial.print(" Magnitude: "); Serial.println(Mag*1000);
  //Serial.print(" A_Pitch: "); Serial.println(A_Pitch); - NO
  delay(100);

}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AX_raw = (((int16_t)Wire.read()<<8) | Wire.read());
  AY_raw = (((int16_t)Wire.read()<<8) | Wire.read());
  AZ_raw = (((int16_t)Wire.read()<<8) | Wire.read());
  Temp_raw = (((int16_t)Wire.read()<<8) | Wire.read());
  GX_raw = (((int16_t)Wire.read()<<8) | Wire.read());
  GY_raw = (((int16_t)Wire.read()<<8) | Wire.read());
  GZ_raw = (((int16_t)Wire.read()<<8) | Wire.read());
}

//configure MPU6050
void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}
