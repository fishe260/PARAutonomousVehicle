void InitializeRadio() {
  printf_begin();
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setRetries(1, 15);
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(pipe);
  radio.setChannel(42);
  //radio.printDetails();
}

void sendData() {
  RFT_MCStatus = radio.write(&dataTx, sizeof(MyDataTx));
}

void recvData() {
  //While radio available, read data
  bool done = false;
 
  if (radio.isAckPayloadAvailable()) {
    
    radio.read(&ackpayload, sizeof(AutoAckPayload));
    
    LastRecvTime = millis();
    ppsCounter++;
    
    telRadioStatus = true;
    if(ackpayload.bitflag < 128 && telRadioStatus)
      ackpayload.bitflag += 128;
  }
  
  //If lose radio contact, print "Connection Lost"
  if (millis() - LastRecvTime >= 1000) {
    ackpayload.desiredSpeed = 0;
    ackpayload.actualSpeed = 0;
    ackpayload.desiredSteeringAngle = 0;
    ackpayload.actualSteeringAngle = 0;
    ackpayload.voltage = 0;
    ackpayload.current = 0;
    ackpayload.mode = 0;
    ackpayload.RFTPPS = 0;
    ackpayload.bitflag = 0;
  }
  
  //Count Packets per Second
  unsigned long now = millis();
  if (now - lastPPS > 1000) {
    RFT_MCPPS = ppsCounter;
    ppsCounter = 0;
    lastPPS = now;
  }
}
