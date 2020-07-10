#ifndef _I2CDEV_H_
#define _I2CDEV_H_

#ifdef ARDUINO
    #if ARDUINO < 100
        #include "WProgram.h"
    #else
        #include "Arduino.h"
    #endif
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        #include <Wire.h>
    #endif
#else
    #include "ArduinoWrapper.h"
#endif

#define I2CDEV_DEFAULT_READ_TIMEOUT     1000

#define BUFFER_LENGTH 64

class I2Cdev {
    public:
        I2Cdev();
        
        static int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
        static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
        
        static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
        static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);

        static uint16_t readTimeout;
};

#endif /* _I2CDEV_H_ */
