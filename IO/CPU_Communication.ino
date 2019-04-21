/**********
  Subsystem: CPU Communication
  Description: Sends and recieves to/from CPU through I2C protocol.
**********/
#define SLAVE_ADDRESS 0x04 //Address of the Arduino
#define CPU_DATA_LENGTH 3 //How many values CPU transmits
#define DATA_BUFFER_LENGTH 4 //Length (bytes) of each piece of data

byte cpuDataRecv[CPU_DATA_LENGTH];
byte cpuDataSend[DATA_BUFFER_LENGTH];
uint32_t lastCPURecvTime = 0; //Must be global because safety tab uses it

void initCPU() {
  Wire.begin(SLAVE_ADDRESS); //Initialize arduino as slave
  Wire.onRequest(sendCPUData);
  Wire.onReceive(receiveCPUData);
  Serial.println("initing cpu");
}

void sendCPUData() {
  Serial.println("sending");
  //What we send to the CPU
  toByteArray(gpsPacket.lon, cpuDataSend, 4);
  Wire.write(cpuDataSend , 4); //Longitude
  toByteArray(gpsPacket.lat, cpuDataSend, 4);
  Wire.write(cpuDataSend , 4); //Latitude
  toByteArray(gpsPacket.height, cpuDataSend, 4);
  Wire.write(cpuDataSend , 4); //Height
  toByteArray(gpsPacket.hMSL, cpuDataSend, 4);
  Wire.write(cpuDataSend , 4); //hMSL
  toByteArray(gpsPacket.hAcc, cpuDataSend, 4);
  Wire.write(cpuDataSend , 4); //hAcc
  toByteArray(gpsPacket.vAcc, cpuDataSend, 4);
  Wire.write(cpuDataSend , 4); //vAcc
}

void receiveCPUData(int byteCount) {
  Serial.println("recv");
  //What we recieve
  short i = 0;
  while (Wire.available()) {
    cpuDataRecv[i++] = Wire.read();
  }
  lastCPURecvTime = millis();
  if (((programMode == 2 || programMode == 3) || programMode == 4)) {
    //If in auto mode, get desired steering and throttle from CPU, not remote
    desiredThrottle = cpuDataRecv[1]; //cpuDataRecv[0] is garbage?
    desiredSteeringPosition = cpuDataRecv[2];
  }
}

void toByteArray(int input , byte* dataArray, int size) {
  for (byte i = 0 ; i < size ; i++) {
    dataArray[i] = (input >> ((size - i - 1) * 8) ) & 0xFF;
  }
}
