//
//  SerialConnection.cpp
//  PARAutonomousVehicle
//
//  Created by Aidan Fisher on 9/1/20.
//  Copyright © 2020 Aidan Fisher. All rights reserved.
//

#include "SerialConnection.hpp"

SerialCommunication::SerialCommunication(std::string _address, int _baudRate)
{
    address = _address;
    baudRate = _baudRate;
    serialPort = 0; // DEFAULT, can be changed in openSerial()
}

void SerialCommunication::openSerial()
{
    int _serialPort = open(address.c_str(), O_RDWR);
    serialPort = _serialPort;
}

bool SerialCommunication::updateTTY()
{
    // Read in existing settings
    int didRead = tcgetattr(serialPort, tty);
    
    // If the existing settings weren't read, the tcgetattr function
    // Will not return a zero. If that is the case, exit and return false.
    if (didRead != 0)
        return false;
    
    // Set flags
    tty->c_cflag &= ~PARENB; // Clear parity bit
    tty->c_cflag &= ~CSTOPB; // Clear stop field
    tty->c_cflag |= CS8; // Set to 8 bits per word
    tty->c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow
    tty->c_cflag |= CREAD | CLOCAL; // Turn on READ and ignore control lines (CLOCAL = 1)
    
    return true;
}
