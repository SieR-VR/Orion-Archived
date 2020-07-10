#include "I2Cdev.h"

I2Cdev::I2Cdev() {
}

int8_t I2Cdev::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout) {
    return readBytes(devAddr, regAddr, 1, data, timeout);
}

int8_t I2Cdev::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout) {
    int8_t count = 0;
    uint32_t t1 = millis();

    for (uint8_t k = 0; k < length; k += min(length, BUFFER_LENGTH)) {
        Wire.beginTransmission(devAddr);
        Wire.write(regAddr);
        Wire.endTransmission();
        Wire.beginTransmission(devAddr);
        Wire.requestFrom(devAddr, (uint8_t)min(length - k, BUFFER_LENGTH));
        
        for (; Wire.available() && (timeout == 0 || millis() - t1 < timeout); count++) {
            data[count] = Wire.read();
        }
        
        Wire.endTransmission();
    }

    // check for timeout
    if (timeout > 0 && millis() - t1 >= timeout && count < length) count = -1; // timeout
    return count;
}

bool I2Cdev::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    return writeBytes(devAddr, regAddr, 1, &data);
}

bool I2Cdev::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
    uint8_t status = 0;
    Wire.beginTransmission(devAddr);
    Wire.write((uint8_t) regAddr); // send address
    for (uint8_t i = 0; i < length; i++) {
        Wire.write((uint8_t) data[i]);
    }
    status = Wire.endTransmission();
    return status == 0;
}

uint16_t I2Cdev::readTimeout = I2CDEV_DEFAULT_READ_TIMEOUT;
