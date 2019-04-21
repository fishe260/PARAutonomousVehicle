/**********
   Subsystem: IMU (Inertial Measurement Unit)
   Description: Communication via I2C with MPU6050 (Accelerometer, Gyroscope)
   Acceleration Default Sensitivity: +/- 2G

   kartAccelX - Lateral acceleration.
   0 = max accel towards left (left turn)
   255 = max accel towards right (right turn)
   headingGyro = Kart relative heading (degrees)

   kartAccelY - Longitudinal acceleration.
   0 = max accel towards rear (kart slowing down)
   255 = max accel towards front (kart speeding up)

   NOTE: If you change gyro sensitivity, you'll have to make changes in this sketch!
   If you change accelerometer sensitivity, you'll have to change mission control and CPU code!
 *********/
#define IMU_ARRAY_SIZE 64 //Moving average size, Cannot exceed imuPolls

const uint16_t imuGap = 3000; //microseconds between IMU polls
const uint16_t imuPolls = 300; //Initial values to take to calculate offsets

MPU6050 kartIMU;
int16_t accX, accY, accZ, gyroX, gyroY, gyroZ; //16 bit values from MP
int16_t IMUReadings[3][IMU_ARRAY_SIZE]; //For moving average
int32_t IMUOffsets[6]; //For finding offsets

void initIMU() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  kartIMU.initialize();
  IMUStatus = kartIMU.testConnection();
  kartIMU.setFullScaleAccelRange(0); //0 = +/- 2  1 = +/- 4  2 = +/- 8  3 = +/- 16 (g)
  kartIMU.setFullScaleGyroRange(0); //0 = +/- 250  1 = +/- 500  2 = +/- 1000  3 = +/- 2000 (deg/s)

  if (IMUStatus) { //If our IMU isn't starved for power
    //Zero out the IMU
    for (uint16_t i = 0 ; i < imuPolls ; i++) { //Take values
      kartIMU.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);
      if (i < IMU_ARRAY_SIZE) {
        IMUReadings[0][i] = accX;
        IMUReadings[1][i] = accY;
        IMUReadings[2][i] = accZ;
      }
      IMUOffsets[0] += accX;
      IMUOffsets[1] += accY;
      IMUOffsets[2] += accZ;
      IMUOffsets[3] += gyroX;
      IMUOffsets[4] += gyroY;
      IMUOffsets[5] += gyroZ;
    }
    for (uint16_t i = 0 ; i < 6 ; i++) //Average the values
      IMUOffsets[i] /= imuPolls;
  }
}

void getIMUdata() {
  static uint32_t lastIMUTime = 0; //For heading calculation and spacing out requests (micros)
  float gyroHeading = 0;
  float imuTemp = 0;

  //Only poll IMU when the IMU is ready
  if (micros() - lastIMUTime > imuGap && IMUStatus) {
    uint32_t deltaT = micros() - lastIMUTime;
    lastIMUTime = micros();

    //Get values
    kartIMU.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);
    imuTemp = kartIMU.getTemperature();

    //Shift moving average array
    for (int i = 0 ; i < IMU_ARRAY_SIZE - 1 ; i++) {
      for (int j = 0 ; j < 3 ; j++) {
        IMUReadings[j][i] = IMUReadings[j][i + 1];
      }
    }
    IMUReadings[0][IMU_ARRAY_SIZE - 1] = accX;
    IMUReadings[1][IMU_ARRAY_SIZE - 1] = accY;
    IMUReadings[2][IMU_ARRAY_SIZE - 1] = accZ;

    //Calculate moving average
    accX = 0;
    accY = 0;
    accZ = 0;
    for (int i = 0 ; i < IMU_ARRAY_SIZE ; i++) {
      accX += IMUReadings[0][i] / IMU_ARRAY_SIZE;
      accY += IMUReadings[1][i] / IMU_ARRAY_SIZE;
      accZ += IMUReadings[2][i] / IMU_ARRAY_SIZE;
    }

    //Apply offsets
    accX -= IMUOffsets[0];
    accY -= IMUOffsets[1];
    accZ -= IMUOffsets[2] - 16384;
    gyroX -= IMUOffsets[3];
    gyroY -= IMUOffsets[4];
    gyroZ -= IMUOffsets[5];
    if (abs(gyroZ) <= 50) //Ignore steady state jitters (prevents increasing heading error)
      gyroZ = 0;

    //Convert to useable vals
    kartAccelX = map(1 * accY, -32768, 32767, 0, 255);
    kartAccelY = map(-1 * accX, -32768, 32767, 0, 255); //The '-1' is due to the orientation of the chip relative to kart.

    gyroHeading -= deltaT / 1000000.0 * map(gyroZ, -32768, 32767 , -250, 250);
    if (gyroHeading > 360)
      gyroHeading -= 360;
    else if (gyroHeading < 0)
      gyroHeading += 360;
    imuTemp = imuTemp / 340.00 + 36.53; //Convert to Celcius (equation on data sheet)

    if (imuTemp > 50 || !kartIMU.testConnection())
      IMUStatus = false;

    //Uncomment to see raw IMU values
    //Serial.print("IMU Temp (C): ");
    //Serial.println(imuTemp);
    //Serial.print("AccelX (byte): ");
    //Serial.println(kartAccelX);
    //Serial.print("AccelY (byte): ");
    //Serial.println(kartAccelY);
    //Serial.print("Heading (degrees): ");
    //Serial.println(gyroHeading);
    //Serial.print("IMU Status: ");
    //Serial.println(IMUStatus);
  }
}
