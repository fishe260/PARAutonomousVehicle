/**********
   Subsystem: Steering
   Description: Communication to steering motor controller and calculation of desired controller effort through encoder feedback
   Quadrature encoder currently uses only relative position.
 *********/
#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define STEERING_MOTOR_PIN 7
#define ENCODER_PWM_PIN A3

const int16_t zeroPosition = 2950; //Absolute position of encoder for zero steer angle
const int16_t positionRange = 1400; //Steering range in encoder positions (+/-)
const uint16_t kp = 1000; //1000 x Proportional Response
const uint16_t ki = 0; //1000 x Integral Response
const uint16_t kd = 0; //1000 x Derivative Response

Servo steeringMotorController;
volatile uint32_t steerRiseTime; //For absolute encoder
volatile uint16_t startEncoderPos;
volatile int16_t encoderPos; //For Quadrature Encoder
bool A_set = false;
bool B_set = false;

void initSteering() {
  pinMode(ENCODER_A_PIN, INPUT);
  pinMode(ENCODER_B_PIN, INPUT);
  pinMode(STEERING_MOTOR_PIN, OUTPUT);
  pinMode(ENCODER_PWM_PIN, INPUT);
  steeringMotorController.attach(STEERING_MOTOR_PIN);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), incrementencoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), incrementencoderB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PWM_PIN), evalRisingPWM, RISING);

  //Get absolute position
  steerRiseTime = 0;
  delay(10); //Lets the interrupts trigger at least once.

  //This is done because if the wheels are very off kilter, the absolute signal may have looped back to 0.
  if (startEncoderPos < 200)
    startEncoderPos += 4250;

  detachInterrupt(digitalPinToInterrupt(ENCODER_PWM_PIN));

}

void writeSteering() {
  static int32_t errorInteg = 0;
  static int16_t lastError = 0;
  
  //Command byte to encoder ticks
  int16_t desiredEncoderPosition = map(desiredSteeringAngle, 0, 255, zeroPosition - positionRange, zeroPosition + positionRange);
  //Encoder ticks to communication value
  actualSteeringAngle = map(encoderPos + startEncoderPos, zeroPosition - positionRange, zeroPosition + positionRange, 0, 255);

  //PID
  int16_t error = (encoderPos + startEncoderPos) - desiredEncoderPosition;
  errorInteg += error;
  int16_t errorDeriv = error - lastError;
  lastError = error;
  //These numbers mess with controller effort. Making the range too wide will cause a high current draw and could FUCK HARDWARE UP.
  int16_t pid = constrain((kp * error + ki * errorInteg + kd * errorDeriv) / 1000.0 + 1500, 1300, 1700); //Max range: 1000-2000

  //steeringMotorController.writeMicroseconds(pid);
}

void incrementencoderA() {
  A_set = digitalRead(ENCODER_A_PIN) == 1;
  encoderPos += (A_set != B_set) ? +1 : -1;
}

void incrementencoderB() {
  B_set = digitalRead(ENCODER_B_PIN) == 1;
  encoderPos += (A_set == B_set) ? +1 : -1;
}

void evalRisingPWM() {
  steerRiseTime = micros();
  attachInterrupt(digitalPinToInterrupt(ENCODER_PWM_PIN), evalFallingPWM, FALLING);
}

void evalFallingPWM() {
  startEncoderPos = micros() - steerRiseTime;
  attachInterrupt(digitalPinToInterrupt(ENCODER_PWM_PIN), evalRisingPWM, RISING);
}
