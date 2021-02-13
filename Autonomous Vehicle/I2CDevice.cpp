//
//  I2CDevice.cpp
//  Autonomous Vehicle
//
//  Created by Aidan Fisher on 1/30/21.
//

#include "I2CDevice.hpp"

#define I2C_SLAVE 1 // TODO: DELETE THIS (ONLY USED TO COMPILE FOR MAC)

int I2CDevice::i2cFile = -1; // initialization value

I2CDevice::I2CDevice(int _address)
{
    address = _address; 
}

bool I2CDevice::openI2C(std::string i2cFilename) // static
{
    int _i2cFile = open(i2cFilename.c_str(), O_RDWR);
    
    // Check if the file can be opened
    if (_i2cFile < 0)
    {
        return false;
    }
    
    i2cFile = _i2cFile;
    return true;
}

bool I2CDevice::openDevice()
{
    // Check whether or not we can talk to the bus
    if (ioctl(i2cFile, I2C_SLAVE, address) < 0)
    {
        return false;
    }
    
    return true;
}

bool I2CDevice::readBytes(long numBytesToRead)
{
    buffer = (char*) malloc(sizeof(char) * numBytesToRead);
    long numBytesRead = read(i2cFile, buffer, numBytesToRead);
    if (numBytesRead != numBytesToRead)
    {
        free(buffer);
        return false;
    }
    return true;
}

bool I2CDevice::freeBuffer()
{
    // if the buffer does not hold anything, return false
    if (buffer == nullptr)
        return false;
    
    free(buffer);
    return true;
}

char* I2CDevice::getBuffer()
{
    return buffer; 
}

void I2CDevice::setAddress(int _address)
{
    this->address = _address;
}

void I2CDevice::closeI2C()
{
    if (i2cFile > 0)
    {
        close(i2cFile);
        i2cFile = -1;
    }
}

I2CDevice::~I2CDevice()
{
    freeBuffer();
}
