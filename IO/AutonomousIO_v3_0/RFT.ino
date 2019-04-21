/**********
   Subsystem: RFT (Radio Frequency Telemetry)
   Description: Telemetry communication from kart to Mission Control.
   Packet to Mission Control details:
   byte desiredThrottle -- input throttle value
   uint16_t actualSpeed -- kart speed
   byte desiredSteeringAngle -- self explanatory
   byte actualSteeringAngle -- self explanatory
   uint16_t voltage -- battery voltage
   uint16_t current -- battery current
   byte kartAccelX -- Lateral acceleration (side-side)
   byte kartAccelY -- Longitudinal acceleration (front-back)
   byte mode -- current program mode
   uint16_t RFTPPS -- how many packets per second the kart is sending/recieving
   uint16_t CPUIPS -- how many images per second the CPU is processing
   byte bitflag -- subsystem status code
   Total packet size: N$I bytes
 *********/
const uint64_t pipe = 0xF0F0F0F0E1LL;
RF24 radio(9, 10); //CE, CSN

//The size of this structure should not exceed 32 bytes
//Data to send to Mission Control.
struct AutoAckPayload {
  byte desiredThrottle;
  int16_t actualSpeed;
  byte desiredSteeringAngle;
  byte actualSteeringAngle;
  uint16_t voltage;
  uint16_t current;
  byte kartAccelY;
  byte kartAccelX;
  byte programMode;
  uint16_t RFTPPS;
  uint16_t CPUIPS; //CPU Images per second
  byte bitflag;
};
AutoAckPayload MCPacket;

//Data to recieve from Mission Control
struct MyDataRx {
  bool remoteStop;
};
MyDataRx MCCommands;

uint16_t packetCounterRFT; //Counts amount of packets per second
uint32_t lastTimeRFT, lastTimeRFTPPS; //Used for radio timeout and PPS counting respectivley.

void initRFT() {
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setRetries(0, 3); //Delay between retries, amount of retries before giving up
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  radio.setChannel(42);
  telRadioStatus = true;
}

void getRFTdata() {
  MCPacket.desiredThrottle = desiredThrottle;
  MCPacket.actualSpeed = actualSpeed;
  MCPacket.desiredSteeringAngle = desiredSteeringAngle;
  MCPacket.actualSteeringAngle = actualSteeringAngle;
  MCPacket.voltage = kartVoltage;
  MCPacket.current = kartCurrent;
  MCPacket.kartAccelX = kartAccelX;
  MCPacket.kartAccelY = kartAccelY;
  MCPacket.programMode = programMode;
  MCPacket.RFTPPS = RFTPPS;
  MCPacket.CPUIPS = CPUIPS;
  MCPacket.bitflag = bitflag;

  //Uncomment this to see the payload being sent to MC
  /*
    Serial.print("Sending payload: ");
    Serial.print(MCPacket.desiredThrottle);
    Serial.print(',');
    Serial.print(MCPacket.actualSpeed);
    Serial.print(',');
    Serial.print(MCPacket.desiredSteeringAngle);
    Serial.print(',');
    Serial.print(MCPacket.actualSteeringAngle);
    Serial.print(',');
    Serial.print(MCPacket.voltage);
    Serial.print(',');
    Serial.print(MCPacket.current);
    Serial.print(',');
    Serial.print(MCPacket.kartAccelY);
    Serial.print(',');
    Serial.print(MCPacket.kartAccelX);
    Serial.print(',');
    Serial.print(MCPacket.programMode);
    Serial.print(',');
    Serial.print(MCPacket.RFTPPS);
    Serial.print(',');
    Serial.print(MCPacket.CPUIPS);
    Serial.print(',');
    Serial.print(MCPacket.bitflag);
    Serial.print(',');
    Serial.println('*');
  */

  if (radio.available()) {
    radio.writeAckPayload(1, &MCPacket, sizeof(AutoAckPayload));
    radio.read(&MCCommands, sizeof(MyDataRx));
    lastTimeRFT = millis();
    packetCounterRFT++;
    telRadioStatus = true;
    //If telRadioStatus was false, change that.
    if (MCPacket.bitflag < 128 && telRadioStatus)
      MCPacket.bitflag += 128;
  }

  //Loss of radio contact for 0.5 seconds
  if (millis() - lastTimeRFT >= 500) {
    telRadioStatus = false;
    if (MCPacket.bitflag >= 128)
      MCPacket.bitflag -= 128;
  }

  //Count Packets per Second
  if (millis() - lastTimeRFTPPS >= 1000) {
    RFTPPS = packetCounterRFT;
    packetCounterRFT = 0;
    lastTimeRFTPPS = millis();
  }
}
