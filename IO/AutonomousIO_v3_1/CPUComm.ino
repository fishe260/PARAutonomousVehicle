/**********
  Subsystem: CPU Communication
  Description: Sends and recieves to/from CPU through I2C protocol.
**********/
#define SLAVE_ADDRESS 0x04 //Address of the Arduino
#define CPU_DATA_LENGTH 3 //How many values CPU transmits

byte cpuData[CPU_DATA_LENGTH];
uint32_t lastCPURecvTime = 0; //Must be global because safety tab uses it

void initCPU() {
  Wire.begin(SLAVE_ADDRESS); //Initialize arduino as slave
  Wire.onRequest(sendCPUData);
  Wire.onReceive(receiveCPUData);
}

void sendCPUData() {
  //What we send to the CPU
  Wire.write(desiredThrottle);
  Wire.write(desiredSteeringAngle);
}

void receiveCPUData(int byteCount) {
  //What we recieve
  short i = 0;
  while (Wire.available()) {
    cpuData[i++] = Wire.read();
  }
  lastCPURecvTime = millis();
  if (((programMode == 2 || programMode == 3) || programMode == 4)) {
    //If in auto mode, get desired steering and throttle from CPU, not remote
    desiredThrottle = cpuData[1]; //cpuData[0] is garbage?
    desiredSteeringAngle = cpuData[2];
  }
}
