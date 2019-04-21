/*
MissionControl Front-End
To-do:
Get Speed vals
Live FPV feed
Optimize
Fix File Recording-- Only create file on record, only append to the file if already created. 

If testing without radio:
1) Comment "One"
2) Comment "Two"
3) Uncomment "Three"
4) Comment "Four"
*/

import processing.serial.*;
import processing.video.*;

Serial RFTSerial; //Serial Ports
Capture fpvCam; //FPV Cam

//Background image
PImage background; 

//Kart Packet
float throttle, actualSpeed, desiredSteeringAngle, actualSteeringAngle, voltage, current; 
int mode, RFTPPS, CPUIPS;
byte bitflag;

//Fonts (size 16 and 24)
PFont fontSmall, fontLarge;
int smallSize, largeSize;

//Dial placement
int leftX, numberOfDials, centerDialY, dialDiam, dialSpacing, n; 

//Dial Range Settings
int maxSpeed, maxVoltage, minVoltage, maxCurrent, maxPower, maxPPS, maxCPUIPS; 

//Dial Values (not in packet)
float power;
int brakeVal, throttleVal; //0 - 100 percent

//Dual-meter placement
int rectLength, rectWidth, rectMeterX, rectMeterY, topAdjust, bottomAdjust, wheelDiam; 

//Dual-meter Range Settings
int minSteerAngle, maxSteerAngle;

//Energy Display
long lastTime;
float energyConsumed, batteryHealth; 
String string1;

//Mode Display
String[] programModes = {"0-Inactive", "1-RC", "2-Training", "3-Time Trial", "4-Race", "5-Unused", "6-Unused", "7-Waiting for controller confirm"}; 

//Indicator Block placement
int indicatorStartX, indicatorStartY, indicatorDiameter, indicatorSpacing, biggest; 

//Indicator Block Text
String[] labels = {"CPU", "STEERING", "THROTTLE", "COMM RADIO", "LOCATION", "POWER", "IMU", "TEL RADIO"}; 

//Steering Wheel Image
PImage indicator;

//IMU Circle diameter
int imuDiam;

//IMU Values
float IMUSensitivity; //IMU data range. Default for MPU6050 is +/- 2G
float maxAccel, xAccel, yAccel;

//Button size
int stopDiam, recDiam;
boolean stopToggle, recToggle;

//File recording
PrintWriter fileOutput;

//FPS Indicator
long lastFPSTime;
int FPSCounter, FPSCounterLabel;
 
//For testing without radio
int i = 0;
boolean flip = false;
 
void setup() {
  printArray(Serial.list());
  //RFTSerial = new Serial(this, Serial.list()[0], 115200); //One
  //RFTSerial.bufferUntil('\n'); //Two
 
  
  //printArray(Capture.list());
  //fpvCam = new Capture(this, Capture.list()[101] );
  //fpvCam.start();
 
  fullScreen(); //Switch between full screen and windowed
  //size(1920,1080);
  background = loadImage("Background3.jpg");
  background.resize(width, height);
  
  smallSize = width/100;
  largeSize = width/60;
  fontSmall = createFont("Lucida Console", smallSize);
  fontLarge = createFont("Lucida Console", largeSize);
  
  energyConsumed = 0;
  batteryHealth = 48 * 71; //Battery capacity 
  maxSpeed = 50; //Mph
  minVoltage = 30; //Volts
  maxVoltage = 60;
  maxCurrent = 300; //Amps
  maxPower = (int)(maxVoltage*maxCurrent/745.7);
  maxPPS = 500; //Packets per sec
  maxCPUIPS = 300; //Images per second
  
  minSteerAngle = -60; //Degrees
  maxSteerAngle = 60;
  
  indicator = loadImage("SWheel.png");
  wheelDiam = height/10;
  indicator.resize(wheelDiam , wheelDiam);
  
  IMUSensitivity = 2; //Gs (# range for IMU)
  imuDiam = (int) (width / 7.5);
  maxAccel = 1; //Gs (max on dial)
  xAccel = 0; 
  yAccel = 0;
  
  lastFPSTime = millis();
  FPSCounter = 0;
  FPSCounterLabel = 0;
  
  stopDiam = height/7;
  recDiam = height/12;
  stopToggle = false;
  recToggle = false;
  
  fileOutput = createWriter(year() + "-" + month() + "-" + day() + "_" + "KartData.csv");
  fileOutput.println("Time, Throttle, Actual Speed, Desired Steering, Actual Steering, Voltage, Current, yAccel, xAccel, Mode, RFT PPS, CPU IPS, Bitflag");
}

void draw() {  
  //Three
  //if(i == 0 || i == 255)
  //  flip = !flip;
  //if (flip)
  //  desiredSteeringAngle = i++;
  //else 
  //  desiredSteeringAngle = i--;
  
  //sendData(); //Four
  
  image(background, 0,0); //Switch between image and solid color for background
  //fpvCam.resize(width/3, height/3);
  //image(fpvCam, width/3, height/3);
  //background(0);
  
  //CALCULATE POWER
  power = (voltage/100*current/100)/1000; //kiloWatts

  //CALCULATE ENERGY CONSUMED
  energyConsumed += (power) * (lastTime-millis()) * (1 / 1000) * (1 / 3600); //Kwatt milli seconds -> kiloWatt seconds -> kiloWatt seconds - > kilowatt hours
   
  //UPPER DIALS BLOCK
  dialBlock();
  
  //DRAW STEERING ANGLE METER
  steerMeter();
  
  //DRAW TEXT BLOCK
  textBlock();
  
  //DRAW SYSTEM STATUS INDICATORS
  statusIndicatorBlock();
  
  //DRAW IMU INDICATOR
  accelMeter();
  
  //DRAW COMMAND BUTTONS
  commands();
  
  //DRAW FPS INDICATOR
  fps();
    
  lastTime = millis();  
}

void stop(){
  fileOutput.flush();  // Writes the remaining data to the file
  fileOutput.close();  // Finishes the file
  fpvCam.stop();
  exit();  // Stops the program
} 
