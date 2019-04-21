void writeThrottle() {
  int throttle_val = map(desiredSpeed,1000,2000,-REVLIM,THROTTLELIM);
  
  if (throttle_val > 20) {
    digitalWrite(FWD,1);
    digitalWrite(REV,0);
    analogWrite(driveMotor,abs(throttle_val));
  }
  else if (throttle_val < -20) {
    digitalWrite(FWD,0);
    digitalWrite(REV,1);
    analogWrite(driveMotor,abs(throttle_val));
  }
  else {
    digitalWrite(FWD,1);
    digitalWrite(REV,0);
    analogWrite(driveMotor,0);
  }
}


void initThrottle() {
  //TCCR0B = (TCCR0B & 0b11111000) | 0x01;    //CHANGE PWM FREQ
  pinMode(FWD, OUTPUT);
  pinMode(REV, OUTPUT);
  analogWrite(footbrake,255);
  analogWrite(driveMotor,0);
}
