#include <Arduino.h>
#include <BLEPeripheral.h>
#include "../lib/MPU9250/MPU9250_9Axis_MotionApps41.h"

#define BLE_REQ   10
#define BLE_RDY   2
#define BLE_RST   9

MPU9250 mpu;
BLEPeripheral peripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService service = BLEService("0x183B");
BLECharacteristic characteristic = BLECharacteristic("0x183C", BLERead, 8);

bool dmpready = false;

uint8_t mpuIntStatus;
uint16_t packetSize;    
uint16_t fifoCount;     
uint8_t fifoBuffer[64];

bool devstatus;
volatile bool mpuInterrupt = false;  

void dmpDataReady() {
    mpuInterrupt = true;
}

void setup() {
  peripheral.setLocalName("Orion_Tracker");
  peripheral.setDeviceName("Orion_Tracker");
  
  //peripheral.setConnectionInterval(0x0006,0x0060);

  peripheral.setAdvertisedServiceUuid(service.uuid());
  peripheral.addAttribute(service);
  peripheral.addAttribute(characteristic);

  peripheral.begin();

  Serial.begin(9600);
  Serial.print("asdf");

  Wire.begin();
  mpu.initialize();
  devstatus = mpu.dmpInitialize();
  if(devstatus == 0) {
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    dmpready = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
}

void loop() {
  if(peripheral.connected()) {
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    fifoCount = mpu.getFIFOCount();
    if((mpuIntStatus & 0x10) || fifoCount == 1024) {
      mpu.resetFIFO();
    } 
    else if(mpuIntStatus & 0x01) {
      while(fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      fifoCount -= packetSize;

      char packet[] = {fifoBuffer[0], fifoBuffer[1], fifoBuffer[2], fifoBuffer[3], fifoBuffer[4], fifoBuffer[5], fifoBuffer[6], fifoBuffer[7]};
      characteristic.setValue(packet);
      Serial.print(packet);
    }
  }
  peripheral.poll();
}