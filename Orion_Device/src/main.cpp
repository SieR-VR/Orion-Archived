#include <Arduino.h>

#include <SPI.h>
#include <BLEPeripheral.h>

#include "I2Cdev.h"
#include "MPU9250_REG.h"

#define MPU9250_ADDRESS 0x68

uint8_t accelData[6] = { 0 };
uint8_t gyroData[6] = { 0 };
uint8_t magData[6] = { 0 };

uint8_t destination[3];

uint32_t time;

BLEPeripheral trackerPeripheral = BLEPeripheral();

BLEService service = BLEService("0000183B-0000-1000-8000-00805F9B34FB");
BLEFixedLengthCharacteristic accelCharacteristic = BLEFixedLengthCharacteristic("faf3ee90-c260-11ea-b3de-0242ac130004", BLENotify | BLERead, 6);
BLEFixedLengthCharacteristic gyroCharacteristic = BLEFixedLengthCharacteristic("faf3ee91-c260-11ea-b3de-0242ac130004", BLENotify | BLERead, 6);
BLEFixedLengthCharacteristic magCharacteristic = BLEFixedLengthCharacteristic("faf3ee92-c260-11ea-b3de-0242ac130004", BLENotify | BLERead, 6);

I2Cdev i2cInterface = I2Cdev();

void initMPU9250();
void initAK8963();

void readAccelData();
void readGyroData();
void readMagData();

void setup()
{
  Wire.begin();
  Wire.setClock(400000);

  initAK8963();
  initMPU9250();

  BLEDescriptor magDescriptor = BLEDescriptor("faf3ee93-c260-11ea-b3de-0242ac130004", destination, 3);

  trackerPeripheral.setLocalName("Orion_Tracker");
  trackerPeripheral.setDeviceName("Orion_Tracker");

  trackerPeripheral.setConnectionInterval(0x0006, 0x0060);
  trackerPeripheral.setTxPower(4);

  trackerPeripheral.setAdvertisedServiceUuid(service.uuid());
  trackerPeripheral.addAttribute(service);
  trackerPeripheral.addAttribute(accelCharacteristic);
  trackerPeripheral.addAttribute(gyroCharacteristic);
  trackerPeripheral.addAttribute(magCharacteristic);
  trackerPeripheral.addAttribute(magDescriptor);
  
  trackerPeripheral.begin();

  Serial.begin(9600);
}

void loop() {
  readAccelData();
  readGyroData();
  readMagData();

  accelCharacteristic.setValue((char *)accelData);
  gyroCharacteristic.setValue((char *)gyroData);
  magCharacteristic.setValue((char *)magData);

  trackerPeripheral.poll();
  Serial.print((int16_t)(magData[0] << 8 | magData[1])); Serial.print(" ");
  Serial.print((int16_t)(magData[2] << 8 | magData[3])); Serial.print(" ");
  Serial.println((int16_t)(magData[4] << 8 | magData[5]));
}

void initMPU9250() {
  i2cInterface.writeByte(MPU9250_ADDRESS, MPU9250_RA_PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors 
  delay(100);

  i2cInterface.writeByte(MPU9250_ADDRESS, MPU9250_RA_PWR_MGMT_1, 0x01); // Auto select clock source to be PLL gyroscope reference if ready else
  delay(200);

  i2cInterface.writeByte(MPU9250_ADDRESS, MPU9250_RA_CONFIG, 0x03);
  i2cInterface.writeByte(MPU9250_ADDRESS, MPU9250_RA_SMPLRT_DIV, 0x00);

  uint8_t gyroConfig;
  i2cInterface.readByte(MPU9250_ADDRESS, MPU9250_RA_GYRO_CONFIG, &gyroConfig);
  gyroConfig &= ~0x02;
  gyroConfig &= ~0x18;
  gyroConfig |= 0x13;
  i2cInterface.writeByte(MPU9250_ADDRESS, MPU9250_RA_GYRO_CONFIG, gyroConfig);

  uint8_t accelConfig;
  i2cInterface.readByte(MPU9250_ADDRESS, MPU9250_RA_ACCEL_CONFIG, &accelConfig);
  accelConfig &= ~0x18;
  accelConfig |= 0x10;
  i2cInterface.writeByte(MPU9250_ADDRESS, MPU9250_RA_ACCEL_CONFIG, accelConfig);

  uint8_t accelConfig2;
  i2cInterface.readByte(MPU9250_ADDRESS, MPU9250_RA_FF_THR, &accelConfig2);
  accelConfig &= ~0x0F;
  accelConfig |= 0x08;
  i2cInterface.writeByte(MPU9250_ADDRESS, MPU9250_RA_FF_THR, accelConfig2);

  i2cInterface.writeByte(MPU9250_ADDRESS, MPU9250_RA_FIFO_EN, 0x00);
}

void initAK8963() {
  i2cInterface.writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
  i2cInterface.writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
  i2cInterface.readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, destination);  // Read the x-, y-, and z-axis calibration values

  i2cInterface.writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
  i2cInterface.writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x11); // Set magnetometer data resolution and sample ODR
}

void readAccelData() {
  i2cInterface.readBytes(MPU9250_ADDRESS, MPU9250_RA_ACCEL_XOUT_H, 6, accelData);
}

void readGyroData(){
  i2cInterface.readBytes(MPU9250_ADDRESS, MPU9250_RA_GYRO_XOUT_H, 6, gyroData);
}

void readMagData() {
  uint8_t magStatus;
  uint8_t tempMagData[6];
  i2cInterface.readByte(AK8963_ADDRESS, AK8963_ST1, &magStatus);
  if(magStatus & 0x01) {
    i2cInterface.readBytes(AK8963_ADDRESS, AK8963_XOUT_L, 6, tempMagData);
    magData[0] = tempMagData[1];
    magData[1] = tempMagData[0];
    magData[2] = tempMagData[3];
    magData[3] = tempMagData[2];
    magData[4] = tempMagData[5];
    magData[5] = tempMagData[4];
  }
  i2cInterface.writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x11); // Set magnetometer data resolution and sample ODR
}