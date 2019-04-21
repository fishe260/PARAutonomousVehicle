/*
void writeSerial() {
  Serial.print(desiredSpeed);
  Serial.print(',');
  Serial.print(desiredSteeringAngle);
  Serial.print(',');
  Serial.print(posllh.CurrentLAT);
  Serial.print(',');
  Serial.print(posllh.CurrentLONG);
  Serial.print(',');
  Serial.println(heading);
  //Serial.print(',');
  //Serial.print('\n');
}

void readSerial() {
  String str = Serial.readStringUntil('*');
  String throttleStr = str.substring(str.indexOf('!'),str.indexOf('?'));
  String steeringStr = str.substring(str.indexOf('?'),str.indexOf('*'));
  throttleStr.remove(0);
  steeringStr.remove(1);
  OutputThrottleRPI = throttleStr.toInt();
  desiredSteeringAngleRPI = steeringStr.toInt();
}
*/
