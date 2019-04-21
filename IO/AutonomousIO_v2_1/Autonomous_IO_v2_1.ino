#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Wire.h>
#include <SPI.h>
#include "printf.h"
#include <Servo.h>

//#define DEBUG


//RFC
#define RFCpin 4
#define NUM_CHANNELS 8    // we are working with an 8-ch-Transmitter
#define NUM_AVG 3     // 3 added values for average
volatile unsigned long timeLast;
volatile int ch[NUM_CHANNELS+1];
volatile int valuesInt[NUM_CHANNELS+1];
volatile byte average = NUM_AVG;
volatile byte RFCcounter = NUM_CHANNELS;
byte pulse = 0;
int mode = 0;
bool commRadioStatus = false;



//THROTTLE
#define driveMotor 5
#define footbrake 6
#define BRAKELIM 255
#define THROTTLELIM 255
#define REVLIM 255
#define FWD 8
#define REV 11
int desiredSpeed = 1500;
int voltage = 0;
int current = 0;
int OutputThrottleRPI = 1500;


//STEERING
Servo motorController;
#define encoderA 2
#define encoderB 3
#define steeringMotor 7
#define ticksPerRev 4105  //CHANGE
#define LOWER_LIMIT -1150
#define UPPER_LIMIT 1150
//Command
int desiredSteeringAngle = 1500;
int desiredSteeringAngleRPI = 1500;
int actualSteeringAngle = 0;
byte actualSteeringAngle_send = 127;

//Encoder
volatile long encoderPos = 0;
boolean A_set = false;
boolean B_set = false;
//PID
float lastError, errorInteg, errorDeriv;
float Kp = 0.3;
float Ki = 0;
float Kd = 0;
boolean encoderStatus = true;


//RFT
const uint64_t pipe = 0xF0F0F0F0E1LL;
RF24 radio(9, 10);    //CE, CSN

//The sizeof this structure should not exceed 32 Bytes
struct MyDataRx {
  boolean remoteStop;
};

MyDataRx dataRx;

struct AutoAckPayload {
  byte desiredSpeed;
  int actualSpeed;
  byte desiredSteeringAngle;
  int actualSteeringAngle;
  int voltage;
  int current;
  byte mode;
  int RFTPPS;
  byte bitflag;
};

AutoAckPayload ackpayload;

boolean telRadioStatus = false;
int ppsCounter;
unsigned long lastPPS, LastRecvTime;


//IMU
#define MPU_addr 0x68
int16_t AccX, AccY, AccZ, t, GyroX, GyroY, GyroZ;
float Temp;


//COMPASS
#define HMC5883L_ADDR 0x1E //0011110b, I2C 7bit address of HMC5883
bool haveHMC5883L = false;
float heading, headingDegrees;
bool detect;
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

//GPS
const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };

struct NAV_POSLLH {
  unsigned char cls;
  unsigned char id;
  unsigned short len;
  unsigned long iTOW;
  long CurrentLONG;
  long CurrentLAT;
  long CurrentALT;
  long hMSL;
  unsigned long hAcc;
  unsigned long vAcc;
};

NAV_POSLLH posllh;

double currentLat, currentLong, currentAlt;



void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  initSteering();
  initRFC();
  initThrottle();
//  initRFT();
  //initIMU();
  initCompass();
}

void loop() {
  getRFCdata();
  //getIMUdata();
  readGPS();
  readCompass();
 
  recvWithStartEndMarkers();    //Get data from RPI and talk back
  
  if (mode) {
    desiredSteeringAngle = desiredSteeringAngleRPI;
    //desiredSpeed = OutputThrottleRPI;
  }
  
  if (!encoderStatus || !commRadioStatus) {
    desiredSpeed = 1500;
    desiredSteeringAngle = 1500;
  }
  
  writeThrottle();
  writeSteering();
  
  //getRFTdata();

  #ifdef DEBUG
    Serial.print(AccX);
    Serial.print(" , ");
    Serial.print(AccY);
    Serial.print(" , ");
    Serial.print(AccZ);
    Serial.print(" , ");
    Serial.print(Temp);
    Serial.print(" , ");
    Serial.println(headingDegrees);
  #endif
}
