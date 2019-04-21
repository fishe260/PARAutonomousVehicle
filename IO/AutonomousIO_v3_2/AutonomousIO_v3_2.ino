/**********
   Tyler's To-Do:
  -CPU Communication
  -Bitflags & contingencies, check contingency effectivness
  -Check N$Is
  -Speed????
  -Location status/optimize

   evGrandPrix Autonomous Kart IO
   v3_2
   Authors: Tyler Mahlmann
   Date: March 2019-
   See metadata.txt for description

   System status bitflag:
   1- CPU Communication
   2- Steering
   4- Throttle
   8- RFC
   16- Location tracking
   32- Power
   64- IMU
   128- Telemetry Radio

   Program Modes: 0- Disabled, 1- RC, 2- Training, 3- Time trial, 4- Race, 5-6 Unused, 7-Waiting on confirmation
   Upon change of mode, kart waits for controller confirm before starting
   (throttle stick held up for at least 1 sec, then back to neutral)
   Debug guide:
   Search "N$I" for features that are not yet implemented
**********/
//Basic arduino libraries
#include <Servo.h>
#include <Wire.h>

//Telemetry radio libraries
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

//Sensor libraries
#include <I2Cdev.h>
#include <MPU6050.h>

//Telemetry variables
byte desiredThrottle = 127;
uint16_t actualSpeed = 0; //N$I
byte desiredSteeringPosition = 127;
byte actualSteeringPosition = 127;
uint16_t kartVoltage = 0;
uint16_t kartCurrent = 0;
byte kartAccelX = 127; //Left-right acceleration @ Default sensitivity: 0 = -2G  127 = 0G  255 = 2G
byte kartAccelY = 127; //Front-back acceleration * (sensitivity / 128 ) + 127
byte programMode = 0;
uint16_t RFTPPS = 0;
uint16_t CPUIPS = 0; //CPU Images per second N$I
byte bitflag = 0;
float heading = 0; //0-360 degrees

//GPS Packet
//This needs to be defined here because it is used by
//CPU_Comm and Arduino compiles tabs alphabetically
struct NAV_gpsPacket {
  unsigned char cls; //Class (should be 0x01)
  unsigned char id; //Identification number of module (should be 0x02)
  uint16_t len; //Length of packet (bytes)
  uint32_t iTOW; //GPS Time of Week (ms)
  int32_t lon; //Longitude (1E7 * degrees)
  int32_t lat; //Latitude (1E7 * degrees)
  int32_t height; //Height above ellipsoid (mm)
  int32_t hMSL; //Height above mean sea level (mm)
  uint32_t hAcc; //Horizontal/Longitude Accuracy Estimate (mm)
  uint32_t vAcc; //Vertical/Latitude Accuracy Estimate (mm)
};
NAV_gpsPacket gpsPacket;
const byte gpsPayloadSize = sizeof(NAV_gpsPacket);

//System Statuses
bool CPUStatus = false;
bool steeringStatus = false; //N$I  //If steering is outside range or large error for certain amount of time
bool throttleStatus = false; //N$I
bool RFCStatus = false;
bool positioningStatus = false;
bool powerStatus = false;
bool IMUStatus = false;
bool telRadioStatus = false;

void setup() {
  uint32_t ioStartTime = millis();
  uint32_t systemStartTime = millis();
  Serial.begin(115200);
  Serial.println("Beginning Initialization...");

  //Initialize subsystems
  initCPU();
  Serial.print("CPU Communication Initialized. Time: ");
  Serial.println(millis() - systemStartTime);
  systemStartTime = millis();
  initSteering();
  Serial.print("Steering initialized. Time: ");
  Serial.println(millis() - systemStartTime);
  systemStartTime = millis();
  initThrottle();
  Serial.print("Throttle initialized. Time: ");
  Serial.println(millis() - systemStartTime);
  systemStartTime = millis();
  initRFC();
  Serial.print("RFC initialized. Time: ");
  Serial.println(millis() - systemStartTime);
  systemStartTime = millis();
  initGPS();
  Serial.print("GPS initialized. Time: ");
  Serial.println(millis() - systemStartTime);
  systemStartTime = millis();
  initPower();
  Serial.print("Power measurement initialized. Time: ");
  Serial.println(millis() - systemStartTime);
  systemStartTime = millis();
  initIMU();
  Serial.print("IMU initialized. Time: ");
  Serial.println(millis() - systemStartTime);
  systemStartTime = millis();
  initRFT();
  Serial.print("RFT initialized. Time: ");
  Serial.println(millis() - systemStartTime);
  systemStartTime = millis();
  Serial.print("Initialization complete. Total Time (ms): ");
  Serial.println(millis() - ioStartTime);
}

void loop() {
  getRFCdata(); //Recieves data from remote
  getIMUdata(); //Gets data from IMU
  if ( ((programMode == 2 || programMode == 3) || programMode == 4)) {
    getLocation(); //If in auto mode, get location
  }

  getLocation();

  getCurrent();
  getVoltage();
  statusChecks(); //See safety tab
  getRFTdata(); //Sends and recieves data with Mission Control

  writeThrottle();
  writeSteering();

  //IO Loops per second calculation
  static uint32_t lastIOTime = 0; //For counting IO LPS (loops per second)
  static uint16_t IOLPS = 0; //IO Loop function Hz
  IOLPS++; //Program loops per second
  if (millis() - lastIOTime >= 1000) {
    lastIOTime = millis();
    Serial.print("LPS: ");
    Serial.println(IOLPS);
    IOLPS = 0;
  }
}
