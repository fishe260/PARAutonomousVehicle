/**********
  Subsystem: Power
  Description: Obtains values for battery current and voltage from Hall effect sensor

  About Voltage Lim: The motor controller already has a low voltage cutoff system.
  But our limit here can let us see that in mission control, and prevent us from drawing
  more juice with the steering/CPU.
  It provides another fail safe to completley cut off power before the batteries are damaged.
**********/
#define VOLT_PIN A0 //A0 or 78
#define VREF_PIN A1 //A1 or 79
#define HALL_PIN A2 //A2 or 80
#define POWER_ARRAY_SIZE 64

const uint16_t currentLim = 200; //(Amps) If we're over 200, we're almost over 10kW
const uint16_t voltageLim = 40; //(Volts) If we're idling under this, we should charge up.
const uint32_t r1 = 2000000; //Bigger resistor
const uint32_t r2 = 100000; //Smaller resistor
const uint16_t currentConst = 320; //Constant for hall effect sensor
const float analogCorrect = 3.3 / 4096; //Maps 0-4096 to voltage

float currentArray[POWER_ARRAY_SIZE]; //Number of vals in RMS calc
uint16_t voltageArray[POWER_ARRAY_SIZE];
float powerOffset;

void initPower() {
  pinMode(VOLT_PIN, INPUT);
  pinMode(VREF_PIN, INPUT);
  pinMode(HALL_PIN, INPUT);
  analogReadResolution(12); //12-bit resolution, 0-3.3V -> 0-4095
  //Calculate offset for zero current value
  float powerSum = 0;
  for (byte i = 0; i < POWER_ARRAY_SIZE; i++) {
    int16_t hallRead = analogRead(HALL_PIN);
    int16_t vrefRead = analogRead(VREF_PIN);
    int16_t voltRead = analogRead(VOLT_PIN);
    currentArray[i] = abs(hallRead - vrefRead) * analogCorrect * currentConst;
    voltageArray[i] = (voltRead * analogCorrect / r2) * (r1 + r2) * 100;
    powerSum += pow(currentArray[i], 2);
  }
  powerOffset = sqrt(powerSum / POWER_ARRAY_SIZE);
}

void getCurrent() {
  powerStatus = true;
  float powerSum = 0;
  //Shift current array
  for (byte i = 0; i < POWER_ARRAY_SIZE - 1; i++) {
    currentArray[i] = currentArray[i + 1];
    powerSum += pow(currentArray[i], 2);
  }
  int16_t hallRead = analogRead(HALL_PIN);
  int16_t vrefRead = analogRead(VREF_PIN);
  currentArray[POWER_ARRAY_SIZE - 1] = abs(hallRead - vrefRead) * analogCorrect * currentConst;

  powerSum += pow(currentArray[POWER_ARRAY_SIZE - 1], 2);
  kartCurrent = (sqrt(powerSum / POWER_ARRAY_SIZE) - powerOffset) * 100.0;

  if (kartCurrent > currentLim * 100)
    powerStatus = false;
}

void getVoltage() {
  int16_t voltRead = analogRead(VOLT_PIN);
  uint32_t voltSum = 0;
  for (int i = 0 ; i < POWER_ARRAY_SIZE - 1; i++) {
    voltageArray[i] = voltageArray[i + 1];
    voltSum += voltageArray[i];
  }
  voltageArray[POWER_ARRAY_SIZE - 1] = (voltRead * analogCorrect / r2) * (r1 + r2) * 100;
  voltSum += voltageArray[POWER_ARRAY_SIZE - 1];
  kartVoltage = voltSum / POWER_ARRAY_SIZE;

  if (kartVoltage < voltageLim * 100 && desiredThrottle == 127) //Idling below volt limit
    powerStatus = false;

  //Uncomment to see raw pin values
  //Serial.print(analogRead(VOLT_PIN));
  //Serial.print(" , ");
  //Serial.print(analogRead(HALL_PIN));
  //Serial.print(" , ");
  //Serial.println(analogRead(VREF_PIN));
  //Serial.println(kartCurrent);
}
