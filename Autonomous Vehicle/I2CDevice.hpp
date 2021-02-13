//
//  I2CDevice.hpp
//  Autonomous Vehicle
//
//  Created by Aidan Fisher on 1/30/21.
//

#ifndef I2CDevice_hpp
#define I2CDevice_hpp

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fstream>

class I2CDevice
{
private:
    int address;
    static int i2cFile;
    char* buffer; 
public:
    I2CDevice(int _address);
    virtual ~I2CDevice();
    static void closeI2C(); 
    static bool openI2C(std::string i2cFilename); 
    bool openDevice();
    void setAddress(int _address);
    bool readBytes(long numBytesToRead);
    bool freeBuffer();
    char* getBuffer(); 
};

#endif /* I2CDevice_hpp */
