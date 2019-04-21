/*****
   Subsystem: RFT_MC (Radio Frequency Telemetry on Mission Control Side)
   Description: A nano/RF24 combo to plug into the mission control computer.
   The radio module recieves data from the kart and communicates it to the mission control software.

   Note: Because the compilers for the Due and Nano treat memory allocation differently, the Due adds
   buffers in a struct. These buffers have been added to this program's struct manually. This likely
   only works for ARM -> ATmega communication!
 *****/
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

//Radio Tranceiver
const uint64_t address = 0xF0F0F0F0E1LL;
RF24 radio(9, 10);

//The sizeof this structure should not exceed 32 Bytes
struct rxStruct {
  byte desiredThrottle;
  uint16_t actualSpeed;
  byte buf1;
  byte desiredSteeringAngle;
  byte actualSteeringAngle;
  uint16_t voltage;
  uint16_t current;
  byte accelX; 
  byte accelY;  
  byte programMode;
  byte buf2;
  uint16_t RFTPPS;
  uint16_t CPUIPS; 
  byte bitflag;
  byte buf3;
};
rxStruct MCPacket;

struct txStruct {
  bool remoteStop;
};
txStruct MCCommands;

uint32_t lastRecvTime;

void setup() {
  Serial.begin(115200);
  initRadio();
}

void loop() {
  recvData(); //Receive Data from Kart
  sendData(); //Send transmitter packet to Kart
  readSerial(); //Recieve data from MissionControl
  writeSerial(); //Send data to MissionControl
}
