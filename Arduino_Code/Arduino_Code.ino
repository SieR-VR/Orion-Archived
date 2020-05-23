#include <BLEPeripheral.h>
#include <Wire.h>
#include "helper_3dmath.h"
#include "MPU9250_9Axis_MotionApps41.h"

#define BLE_REQ   10
#define BLE_RDY   2
#define BLE_RST   9

MPU9250 accelgyro;
BLEPeripheral TrackerPeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService TrackerService = BLEService("C48EF720-3664-4E2B-AD87-BB0553500E8B");
BLEFixedLengthCharacteristic SensorQuat = BLEFixedLengthCharacteristic("C48EF721-3664-4E2B-AD87-BB0553500E8B", BLENotify | BLERead, 8);

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

void setup()
{
  TrackerPeripheral.setLocalName("Tracker");
  TrackerPeripheral.setDeviceName("Tracker");
  TrackerPeripheral.setAdvertisedServiceUuid(TrackerService.uuid());
  TrackerPeripheral.setConnectionInterval(0x0006,0x0010);
  TrackerPeripheral.addAttribute(TrackerService);
  TrackerPeripheral.addAttribute(SensorQuat);
  TrackerPeripheral.begin();
  
  Wire.begin();
  accelgyro.initialize();
  devstatus = accelgyro.dmpInitialize();
  accelgyro.setDLPFMode(0x03);
  if(devstatus == 0) {
    accelgyro.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING);
    mpuIntStatus = accelgyro.getIntStatus();   
    dmpready=true;
    packetSize = accelgyro.dmpGetFIFOPacketSize();
  }
}


String msg = "";
void loop() {  
  
  if(TrackerPeripheral.connected()) {
  
gogo:
  mpuInterrupt = false;
  mpuIntStatus = accelgyro.getIntStatus();
  fifoCount = accelgyro.getFIFOCount();
   
  if((mpuIntStatus & 0x10) || fifoCount == 1024) {
    accelgyro.resetFIFO(); 
    goto gogo; 
  } else if (mpuIntStatus & 0x02); {
    while (fifoCount < packetSize) fifoCount = accelgyro.getFIFOCount();
     accelgyro.getFIFOBytes(fifoBuffer, packetSize);        
     fifoCount -= packetSize;

     char BLEpacket[] = {fifoBuffer[0], fifoBuffer[1], fifoBuffer[4], fifoBuffer[5], fifoBuffer[8], fifoBuffer[9], fifoBuffer[12], fifoBuffer[13]};
     SensorQuat.setValue(BLEpacket);
    }
  }
  TrackerPeripheral.poll();
}
