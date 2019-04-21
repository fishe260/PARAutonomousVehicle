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
#define CURRENT_LIM 200 //(Amps) If we're over 200, we're almost over 10kW
#define VOLTAGE_LIM 40 //(Volts) If we're idling under this, we should charge up.
float powerArray[POWER_ARRAY_SIZE]; //Number of vals in RMS calc
float powerSum;
int16_t hallRead, vrefRead, voltRead; //Stores analog read values
uint32_t r1 = 2000000; //Bigger resistor
uint32_t r2 = 100000; //Smaller resistor
uint32_t currentConst = 320; //Constant for hall effect sensor
float analogCorrect = 3.3 / 4096; //Maps 0-4096 to voltage
float powerOffset;

void initPower() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  analogReadResolution(12); //12-bit resolution, 0-3.3V -> 0-4095
  //Calculate offset for zero current value
  powerSum = 0;
  for (byte i = 0; i < POWER_ARRAY_SIZE; i++) {
    hallRead = analogRead(HALL_PIN);
    vrefRead = analogRead(VREF_PIN);
    powerArray[i] = abs(hallRead - vrefRead) * analogCorrect * currentConst;
    powerSum += pow(powerArray[i], 2);
  }
  powerOffset = sqrt(powerSum / POWER_ARRAY_SIZE);
}

void getCurrent() {
  powerStatus = true;
  powerSum = 0;
  //Shift current array
  for (byte i = 0; i < POWER_ARRAY_SIZE - 1; i++) {
    powerArray[i] = powerArray[i + 1];
    powerSum += pow(powerArray[i], 2);
  }
  hallRead = analogRead(HALL_PIN);
  vrefRead = analogRead(VREF_PIN);
  powerArray[POWER_ARRAY_SIZE - 1] = abs(hallRead - vrefRead) * analogCorrect * currentConst;

  powerSum += pow(powerArray[POWER_ARRAY_SIZE - 1], 2);
  kartCurrent = (sqrt(powerSum / POWER_ARRAY_SIZE) - powerOffset) * 100.0;

  if (kartCurrent > CURRENT_LIM * 100)
    powerStatus = false;
}

void getVoltage() {
  voltRead = analogRead(VOLT_PIN);
  kartVoltage = (voltRead * analogCorrect / r2) * (r1 + r2) * 100;
  if (kartVoltage < VOLTAGE_LIM * 100 && desiredThrottle == 127) //Idling below volt limit
    powerStatus = false;

  //Uncomment to see raw pin values
  //Serial.print(analogRead(VOLT_PIN));
  //Serial.print(" , ");
  //Serial.print(analogRead(HALL_PIN));
  //Serial.print(" , ");
  //Serial.println(analogRead(VREF_PIN));
  //Serial.println(kartCurrent);
}
