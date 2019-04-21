/**********
   Subsystem: RFT (Radio Frequency Telemetry)
   Description: Telemetry communication from kart to Mission Control.

   2-Way communication done with Auto-Acknowledge payloads. The Kart is
   the master, or transmitter. When the slave MC module recieves the packet,
   it sends out an acknowledgemnet packet that contains mission control commands.

   Packet to Mission Control details:
   byte desiredThrottle -- input throttle value
   uint16_t actualSpeed -- kart speed
   byte desiredSteeringPosition -- self explanatory
   byte actualSteeringPosition -- self explanatory
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
const uint64_t address = 0xF0F0F0F0E1LL;

RF24 radio(9, 10); //CE, CSN

//The size of this structure should not exceed 32 bytes
//Data to send to Mission Control.
struct txStruct {
  byte desiredThrottle;
  int16_t actualSpeed;
  byte desiredSteeringPosition;
  byte actualSteeringPosition;
  uint16_t voltage;
  uint16_t current;
  byte kartAccelX;
  byte kartAccelY;
  byte programMode;
  uint16_t RFTPPS;
  uint16_t CPUIPS; //CPU Images per second
  byte bitflag;
};
txStruct MCPacket;

//Data to recieve from Mission Control
struct rxStruct {
  bool remoteStop;
};
rxStruct MCCommands;

void initRFT() {
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(42);
  radio.enableAckPayload();
  radio.setRetries(0, 3); //Delay between retries, amount of retries before giving up
  radio.openWritingPipe(address);
}

void getRFTdata() {
  static uint16_t packetCounterRFT = 0; //Counts amount of packets per second
  static uint32_t lastTimeRFT = 0;
  static uint32_t lastTimeRFTPPS = 0; //Used for radio timeout and PPS counting respectivley.

  MCPacket.desiredThrottle = desiredThrottle;
  MCPacket.actualSpeed = actualSpeed;
  MCPacket.desiredSteeringPosition = desiredSteeringPosition;
  MCPacket.actualSteeringPosition = actualSteeringPosition;
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
    Serial.print(MCPacket.desiredSteeringPosition);
    Serial.print(',');
    Serial.print(MCPacket.actualSteeringPosition);
    Serial.print(',');
    Serial.print(MCPacket.voltage);
    Serial.print(',');
    Serial.print(MCPacket.current);
    Serial.print(',');
    Serial.print(MCPacket.kartAccelX);
    Serial.print(',');
    Serial.print(MCPacket.kartAccelY);
    Serial.print(',');
    Serial.print(MCPacket.programMode);
    Serial.print(',');
    Serial.print(MCPacket.RFTPPS);
    Serial.print(',');
    Serial.print(MCPacket.CPUIPS);
    Serial.print(',');
    Serial.println(MCPacket.bitflag);
  */

  //Serial.println( radio.write(&MCPacket, sizeof(MCPacket)) );
  radio.write(&MCPacket, sizeof(MCPacket));
  if (radio.isAckPayloadAvailable()) {
    telRadioStatus = radio.read(&MCCommands, sizeof(MCCommands));
    lastTimeRFT = millis();
    packetCounterRFT++;
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
