void writeSerial() {
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
  Serial.print(MCPacket.accelX);
  Serial.print(',');
  Serial.print(MCPacket.accelY);
  Serial.print(',');
  Serial.print(MCPacket.programMode);
  Serial.print(',');
  Serial.print(MCPacket.RFTPPS);
  Serial.print(',');
  Serial.print(MCPacket.CPUIPS);
  Serial.print(',');
  Serial.println(MCPacket.bitflag);
}

void readSerial() {
  if (Serial.available()) {
    MCCommands.remoteStop = Serial.read();
  }
}
