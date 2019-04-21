void writeSerial() {
  Serial.print(ackpayload.desiredSpeed);
  Serial.print(',');
  Serial.print(ackpayload.actualSpeed);
  Serial.print(',');
  Serial.print(ackpayload.desiredSteeringAngle);
  Serial.print(',');
  Serial.print(ackpayload.actualSteeringAngle);
  Serial.print(',');
  Serial.print(ackpayload.voltage);
  Serial.print(',');
  Serial.print(ackpayload.current);
  Serial.print(',');
  Serial.print(ackpayload.mode);
  Serial.print(',');
  Serial.print(ackpayload.RFTPPS);
  Serial.print(',');
  Serial.print(RFT_MCPPS);
  Serial.print(',');
  Serial.print(ackpayload.bitflag);
  Serial.print(',');
  Serial.print('*');
}

void recvSerial() {
  if(Serial.available()) {
    dataTx.remoteStop = Serial.read();
  }
}

