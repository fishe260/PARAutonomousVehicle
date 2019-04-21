void getIMUdata() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);   //starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(true);
  Wire.requestFrom(MPU_addr,14,true); // request a total of 14 registers
  AccX = Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AccY = Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AccZ = Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  t = Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyroX = Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyroY = Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyroZ = Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  Temp = t / 340.00 + 36.53;

  flushI2C();
}


void initIMU() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);   //PWR_MGMT_1 register
  Wire.write(0);    //set to zero (wakes up the MPU6050)
  Wire.endTransmission(true);
  flushI2C();
}


void flushI2C() {
  while (Wire.available()) Wire.read();
}

