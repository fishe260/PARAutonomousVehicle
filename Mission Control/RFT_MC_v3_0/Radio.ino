void initRadio() {
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(42);
  radio.enableAckPayload();
  radio.openReadingPipe(1, address);
  radio.writeAckPayload(1, &MCCommands, sizeof(MCCommands)); //Preload data
  radio.startListening();
  lastRecvTime = 0;
}

void sendData() {
  radio.writeAckPayload(1, &MCCommands, sizeof(MCCommands));
}

void recvData() {
  uint32_t currTime = millis();
  if (radio.available()) {
    lastRecvTime = currTime;
    radio.read(&MCPacket, sizeof(MCPacket));
  }

  //Loss of radio contact for 0.5 seconds
  if (currTime - lastRecvTime >= 500) {
    MCPacket.desiredThrottle = 0;
    MCPacket.actualSpeed = 0;
    MCPacket.buf1 = 0;
    MCPacket.desiredSteeringAngle = 0;
    MCPacket.actualSteeringAngle = 0;
    MCPacket.voltage = 0;
    MCPacket.current = 0;
    MCPacket.accelX = 0;
    MCPacket.accelY = 0;
    MCPacket.programMode = 0;
    MCPacket.buf2 = 0;
    MCPacket.RFTPPS = 0;
    MCPacket.CPUIPS = 0;
    MCPacket.bitflag = 0;
    MCPacket.buf3 = 0;
  }
}
