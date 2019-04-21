//Recieve/Transmit telemetry from/to vehicle and relay to MissionControl
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include "printf.h"

//Radio Tranceiver
const uint64_t pipe = 0xF0F0F0F0E1LL;
RF24 radio(9, 10);

//The sizeof this structure should not exceed 32 Bytes
struct MyDataTx {
  boolean remoteStop;
};
MyDataTx dataTx;

struct AutoAckPayload {
  byte desiredSpeed;
  int actualSpeed;
  byte desiredSteeringAngle;
  int actualSteeringAngle;
  int voltage;
  int current;
  byte mode;
  int RFTPPS;
  byte bitflag;
};
AutoAckPayload ackpayload;

int ppsCounter, RFT_MCPPS;
unsigned long lastPPS, LastRecvTime;
bool RFT_MCStatus = false;
boolean telRadioStatus = false;

void setup() {
  Serial.begin(57600);
  InitializeRadio();
}

void loop() {
  //Send transmitter packet to quadrotor receiver
  sendData();

  //Receive Data from Radio
  recvData();

  //Send data to MissionControl
  writeSerial();

  //Recieve data from MissionControl
  recvSerial();
}
