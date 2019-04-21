/**********
   Subsystem: Throttle
   Description: Communication to drive motor controller and brake servo
 *********/
#define DRIVE_MOTOR_PIN 5
#define BRAKE_PIN 12
#define FOOTBRAKE_PIN 6
#define FWD_PIN 8
#define REV_PIN 11

const byte fwdLim = 255; //0-255
const byte revLim = 255;
const uint16_t brakeLim = 1150; //1000-2000, how far brake extends

Servo brakeServo;

void initThrottle() {
  pinMode(DRIVE_MOTOR_PIN, OUTPUT);
  pinMode(FOOTBRAKE_PIN, OUTPUT);
  pinMode(FWD_PIN, OUTPUT);
  pinMode(REV_PIN, OUTPUT);
  pinMode(BRAKE_PIN, OUTPUT);
  brakeServo.attach(BRAKE_PIN);
  digitalWrite(FOOTBRAKE_PIN, 1);
}

void writeThrottle() {
  uint16_t throttleVal = 0; //Value to be written to motor controller
  uint16_t brakeVal = 1000; //Value to brake servo

  if (ch[7] < 1100 && programMode != 0) { //Forward
    digitalWrite(FWD_PIN, 1);
    digitalWrite(REV_PIN, 0);
    if (desiredThrottle > 127) {
      throttleVal = map(desiredThrottle, 128, 255, 0, fwdLim);
    }
    else if (desiredThrottle <= 127) {
      brakeVal = map(desiredThrottle, 127, 0, 1000, brakeLim);
    }
  }
  else if (ch[7] > 1900 && programMode != 0) { //Reverse
    digitalWrite(FWD_PIN, 0);
    digitalWrite(REV_PIN, 1);
    if (desiredThrottle >= 127) {
      brakeVal = map(desiredThrottle, 127, 255, 1000, brakeLim);
    }
    else if (desiredThrottle < 127) {
      throttleVal = map(desiredThrottle, 126, 0, 0, revLim);
    }
  }
  else { //Neutral or EStop
    digitalWrite(FWD_PIN, 1);
    digitalWrite(REV_PIN, 0);
    brakeVal = brakeLim;
  }

  analogWrite(DRIVE_MOTOR_PIN, throttleVal);
  brakeServo.writeMicroseconds(brakeVal);
}
