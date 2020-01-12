
#include <SoftwareSerial.h>
#include <Wire.h>
#include "helper_3dmath.h"
#include "MPU9250_9Axis_MotionApps41.h"

SoftwareSerial bt(7,8);
MPU9250 accelgyro;
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
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();
  accelgyro.initialize();
  devstatus = accelgyro.dmpInitialize();
  accelgyro.setDLPFMode(0x03);
  bt.begin(115200);
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
  while(!bt.available());
  while(bt.read()!= -1);
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
     //byte buf[] = {fifoBuffer[0],fifoBuffer[1],fifoBuffer[4],fifoBuffer[5],fifoBuffer[8],fifoBuffer[9],fifoBuffer[12],fifoBuffer[13]};

     char buff[9] = {fifoBuffer[0],fifoBuffer[1],fifoBuffer[4],fifoBuffer[5],fifoBuffer[8],fifoBuffer[9],fifoBuffer[12],fifoBuffer[13]};

     bt.print(buff);   
     
     //bt.write(buf,8);
  }
}
