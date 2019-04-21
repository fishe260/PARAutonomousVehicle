/**********
   Subsystem: Throttle
   Description: Communication to drive motor controller and brake servo
 *********/
#define DRIVE_MOTOR_PIN 5
#define FOOTBRAKE_PIN 6
#define FWD_PIN 8
#define REV_PIN 11
#define THROTTLELIM 255 //Limits for throttle and brake/reverse, 0-255
#define REVLIM 255
byte throttleVal = 0; //Value to be written to motor controller

void initThrottle() {
  pinMode(FWD_PIN, OUTPUT);
  pinMode(REV_PIN, OUTPUT);
  digitalWrite(FOOTBRAKE_PIN, 1); 
}

void writeThrottle() {
  if (desiredThrottle > 127) {
    //Forward
    throttleVal = map(desiredThrottle, 127, 255, 0, THROTTLELIM);
    digitalWrite(FWD_PIN, 1);
    digitalWrite(REV_PIN, 0);
  }
  else if (desiredThrottle < 127) {
    //Reverse (brake)
    throttleVal = map(desiredThrottle, 126, 0, 0, REVLIM);
    digitalWrite(FWD_PIN, 0);
    digitalWrite(REV_PIN, 1);
  }
  else {
    //Coast
    throttleVal = 0;
  }
  //Serial.print(desiredThrottle);
  //Serial.print(", ");
  //Serial.println(throttleVal);
  analogWrite(DRIVE_MOTOR_PIN, throttleVal);
}
