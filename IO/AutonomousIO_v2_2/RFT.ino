/*
void getRFTdata() {
  //While radio available, read data
    
  if (radio.available()) {
    bool done = false;
    radio.writeAckPayload(1, &ackpayload, sizeof(AutoAckPayload));
    done = radio.read(&dataRx, sizeof(MyDataRx));
    LastRecvTime = millis();
    ppsCounter++;
    telRadioStatus = true;
    if(ackpayload.bitflag < 128 && telRadioStatus)
      ackpayload.bitflag += 128;
  }
  
  //If lose radio contact, print "Connection Lost" and reset data
  if (millis() - LastRecvTime >= 1000) {
    telRadioStatus = false;   
    if(ackpayload.bitflag >= 128 && !telRadioStatus)
      ackpayload.bitflag -= 128;
  }
  
  //Count Packets per Second
  unsigned long now = millis();
  if (now - lastPPS >= 1000) {
    ackpayload.RFTPPS = ppsCounter;
    ppsCounter = 0;
    lastPPS = now;
  }
}


void initRFT() {
  //printf_begin();
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  //radio.setRetries(1, 15);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  radio.setChannel(42);
  //radio.printDetails();
}
*/
