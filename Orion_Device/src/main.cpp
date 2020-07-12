#include <Arduino.h>

#include <SPI.h>
#include <BLEPeripheral.h>

#include "I2Cdev.h"
#include "MPU9250_REG.h"
#include "MadgwickAHRS.h"

#define MPU9250_ADDRESS 0x68

uint8_t accelData[6] = { 0 };
uint8_t gyroData[6] = { 0 };
uint8_t magData[6] = { 0 };

uint8_t destination[3];
float desf[3];

uint32_t time;
bool firstLoop = true;

BLEPeripheral trackerPeripheral = BLEPeripheral();

BLEService service = BLEService("0000183B-0000-1000-8000-00805F9B34FB");
BLEFixedLengthCharacteristic quatCharacteristic = BLEFixedLengthCharacteristic("faf3ee90-c260-11ea-b3de-0242ac130004", BLENotify | BLERead, 16);

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

  for(int i = 0; i < 3; i++)
    desf[i] = ((((destination[0] - 128) * 0.5f ) / 128) + 1);

  trackerPeripheral.setLocalName("Orion_Tracker");
  trackerPeripheral.setDeviceName("Orion_Tracker");

  trackerPeripheral.setConnectionInterval(0x0006, 0x0006);
  trackerPeripheral.setTxPower(4);

  trackerPeripheral.setAdvertisedServiceUuid(service.uuid());
  trackerPeripheral.addAttribute(service);
  trackerPeripheral.addAttribute(quatCharacteristic);
  
  trackerPeripheral.begin();
}

void loop() {
  if(trackerPeripheral.connected()) {
    if(firstLoop) {
      time = millis();
      firstLoop = false;
    }
    readAccelData();
    readGyroData();
    readMagData();

    float ax = (short)(accelData[0] << 8 | accelData[1]) * 8.0f / 32768.0f;
    float ay = (short)(accelData[2] << 8 | accelData[3]) * 8.0f / 32768.0f;
    float az = (short)(accelData[4] << 8 | accelData[5]) * 8.0f / 32768.0f;

    float gx = (short)(gyroData[0] << 8 | gyroData[1]) * 1000.0f / 32768.0f * 0.0349066f;
    float gy = (short)(gyroData[2] << 8 | gyroData[3]) * 1000.0f / 32768.0f * 0.0349066f;
    float gz = (short)(gyroData[4] << 8 | gyroData[5]) * 1000.0f / 32768.0f * 0.0349066f;

    float mx = (short)(magData[0] << 8 | magData[1]) * desf[0] * 300.0f / 4096.0f;
    float my = (short)(magData[2] << 8 | magData[3]) * desf[1] * 300.0f / 4096.0f;
    float mz = (short)(magData[4] << 8 | magData[5]) * desf[2] * 300.0f / 4096.0f;

    MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz, (millis() - time) / 1000.0f);
    time = millis();


    char value[16];
    memcpy(&value[0], &q0, sizeof(float));
    memcpy(&value[4], &q1, sizeof(float));
    memcpy(&value[8], &q2, sizeof(float));
    memcpy(&value[12], &q3, sizeof(float));

    quatCharacteristic.setValue(value);
  }
  trackerPeripheral.poll();
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
  uint8_t cntl;
  i2cInterface.readByte(AK8963_ADDRESS, AK8963_CNTL, &cntl);
  if(cntl == 0x10) {
    i2cInterface.writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x11); // Set magnetometer data resolution and sample ODR
  }
}