/**********
   Tyler's To-Do:
  -CPU Communication
  -Bitflags & contingencies, check contingency effectivness
  -Check N$Is
  -Speed????
  -Seed steering encoder with absolute vals

   evGrandPrix Autonomous Kart IO
   v3_0
   Authors: Tyler Mahlmann
   Date: Aug 2018-Feb 2019
   See metadata .txt file for description

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
//BASIC ARDUINO LIBRARIES
#include <Servo.h>
#include <Wire.h>

//TELEM RADIO LIBRARIES
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

//SENSOR LIBRARIES
#include <I2Cdev.h>
#include <MPU6050.h>

//IO
uint32_t lastIOTime; //For counting IO LPS (loops per second)
uint16_t IOLPS; //IO Loop function Hz

//TELEMETRY VARS
byte desiredThrottle = 127;
uint16_t actualSpeed = 0; //N$I
byte desiredSteeringAngle = 127;
byte actualSteeringAngle = 127;
uint16_t kartVoltage = 0;
uint16_t kartCurrent = 0;
byte kartAccelX = 127; //Left-right acceleration @ Default sensitivity: 0 = -2G  127 = 0G  255 = 2G
byte kartAccelY = 127; //Front-back acceleration * (sensitivity / 128 ) + 127
byte programMode = 0;
uint16_t RFTPPS = 0;
uint16_t CPUIPS = 0; //CPU Images per second N$I
byte bitflag = 0;
float heading = 0; //0-360 degrees

//System Statuses
bool CPUStatus = false; //N$I
bool steeringStatus = false; //N$I  //If steering is outside range or large error for certain amount of time
bool throttleStatus = false; //N$I
bool RFCStatus = false;
bool locationStatus = false; //N$I, if far away from where it started FALSE
bool powerStatus = false;
bool IMUStatus = false;
bool telRadioStatus = false;

void setup() {
  lastIOTime = millis();
  Serial.begin(115200);
  Serial.println("Beginning Initialization...");

  //Initialize subsystems
  initCPU();
  initSteering();
  initThrottle();
  initRFC();
  initPower();
  initIMU();
  initRFT();

  Serial.print("Initialization complete. Time (ms): ");
  Serial.println(millis() - lastIOTime);
}

void loop() {
  getRFCdata(); //Recieves data from remote
  getIMUdata();
  if ( ((programMode == 2 || programMode == 3) || programMode == 4)) {
    //If in auto mode, get location
    getLocation();
  }
  getCurrent();
  getVoltage();

  statusChecks(); //See safety tab
  getRFTdata(); //Sends and recieves data with Mission Control

  writeThrottle();
  writeSteering();

  IOLPS++; //Program loops per second
  if (millis() - lastIOTime >= 1000) {
    lastIOTime = millis();
    Serial.print("LPS: ");
    Serial.println(IOLPS);
    IOLPS = 0;
  }
}
