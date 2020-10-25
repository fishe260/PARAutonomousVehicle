//
//  SerialConnection.hpp
//  PARAutonomousVehicle
//
//  Created by Aidan Fisher on 9/1/20.
//  Copyright © 2020 Aidan Fisher. All rights reserved.
//

#ifndef SerialConnection_hpp
#define SerialConnection_hpp

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string>

class SerialCommunication
{
private:
    std::string address;
    int baudRate;
    int serialPort;
    termios* tty; 

public:
    SerialCommunication(std::string _address, int _baudRate);
    virtual void openSerial();
    virtual bool updateTTY(); 
};
#endif /* SerialConnection_hpp */
