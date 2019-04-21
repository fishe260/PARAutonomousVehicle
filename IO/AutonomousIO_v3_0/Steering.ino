/**********
   Subsystem: Steering
   Description: Communication to steering motor controller and calculation of desired controller effort through encoder feedback
   Quadrature encoder currently uses only relative position.
 *********/
#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define STEERING_MOTOR_PIN 7
#define LOWER_LIMIT -1400 //Encoder position limits
#define UPPER_LIMIT 1400
Servo steeringMotorController;
int16_t desiredEncoderPosition;
volatile int16_t encoderPos = 0;
bool A_set = false;
bool B_set = false;
int16_t error, lastError, errorDeriv, pid;
int32_t errorInteg;
int16_t Kp = 1000; //1000 * Actual Kp
int16_t Ki = 0;
int16_t Kd = 0;

void initSteering() {
  steeringMotorController.attach(STEERING_MOTOR_PIN);
  pinMode(STEERING_MOTOR_PIN, OUTPUT);
  pinMode(ENCODER_A_PIN, INPUT);
  pinMode(ENCODER_B_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), incrementencoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), incrementencoderB, CHANGE);
  //N$I SEED WITH ABSOLUTE VALUE HERE!
}

void writeSteering() {
  //Command byte to encoder ticks
  desiredEncoderPosition = map(desiredSteeringAngle, 0, 255, UPPER_LIMIT, LOWER_LIMIT);
  //Encoder ticks to communication value
  actualSteeringAngle = map(encoderPos, LOWER_LIMIT, UPPER_LIMIT, 255, 0);
  //Serial.println(actualEncoderPosition);

  //PID
  error = desiredEncoderPosition - encoderPos;
  errorInteg += error;
  errorDeriv = error - lastError;
  lastError = error;
  //These numbers mess with controller effort. Making the range too wide will cause a high current draw and could FUCK HARDWARE UP.
  pid = constrain((Kp * error + Ki * errorInteg + Kd * errorDeriv) / 1000.0 + 1500, 1300, 1700); //Max range: 1000-2000
  //Serial.println((Kp * error + Ki * errorInteg + Kd * errorDeriv));

  steeringMotorController.write(pid);
}

void incrementencoderA() {
  A_set = digitalRead(ENCODER_A_PIN) == 1;
  encoderPos += (A_set != B_set) ? +1 : -1;
}

void incrementencoderB() {
  B_set = digitalRead(ENCODER_B_PIN) == 1;
  encoderPos += (A_set == B_set) ? +1 : -1;
}
