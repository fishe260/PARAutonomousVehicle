void writeSteering() {
  //CONVERT DESIRED BYTE TO TICKS
  desiredSteeringAngle = -map(desiredSteeringAngle,1000,2000,LOWER_LIMIT,UPPER_LIMIT);

  actualSteeringAngle = encoderPos;
  
  //PID
  int error = desiredSteeringAngle - actualSteeringAngle;
  errorInteg += error;
  errorDeriv = (error - lastError);
  lastError = error;
  int pid = constrain((Kp*error + Ki*errorInteg + Kd*errorDeriv)+1500,1300,1700);

  if (abs(actualSteeringAngle) > 1200) pid = 1500;

  if (abs(desiredSteeringAngle) < 100) {
    pid = 1500;
    encoderPos = 0;
  }
//
//  #ifdef DEBUG
//    Serial.print(mode);
//    Serial.print(" , ");
//    Serial.print(desiredSpeed);
//    Serial.print(" , ");
//    Serial.print(desiredSteeringAngle);
//    Serial.print(" , ");
//    Serial.print(actualSteeringAngle);
//    Serial.print(" , ");
//    Serial.println(pid);
//  #endif

  //WRITE VALUE TO MOTOR CONTROLLER
  motorController.write(pid);
}

void incrementEncoderA() {
  A_set = digitalRead(encoderA) == 1;
  //A_set = bitRead(PIND, encoderA) == 1;
  encoderPos += (A_set != B_set) ? +1: -1;
}

void incrementEncoderB() {
  B_set = digitalRead(encoderB) == 1;
  //B_set = bitRead(PIND, encoderB) == 1;
  encoderPos += (A_set == B_set) ? +1: -1;
}


void initSteering() {
  motorController.attach(steeringMotor);
  pinMode(steeringMotor, OUTPUT);
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);
  attachInterrupt(encoderA, incrementEncoderA, CHANGE);
  attachInterrupt(encoderB, incrementEncoderB, CHANGE);
}
