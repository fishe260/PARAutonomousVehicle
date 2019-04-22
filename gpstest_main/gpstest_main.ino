//Libraries
#include <Wire.h> //Arduino Library for I2C

//Global
#define SLAVE_ADDRESS 0x04 //Address of the Arduino

//Variables
int data1 = 471234567; //longitude //values to be sent over bus
int data2 = 511111111; //latitude  //names are hardcoded into i2c logoic
int data3 = 355555555; //height    //set the value of these to the values you want to send
int data4 = 123456789; //hMSL
int data5 = 109876543; //hAcc
int data6 = 40; //vAcc

//wire -> wire1 to change i2c lines used

int i = 0; //needed variables for receiving
int number[] = {0,0};

void setup() {
  Serial.begin(9600); //only needed for debugging
  Wire1.begin(SLAVE_ADDRESS); //Initialize arduino as slave 
  Wire1.onRequest(sendTo); //set behavior sendTo when data requested
  Wire1.onReceive(receiveData); //set behavior receiveData  when data recieved 
  pinMode(LED_BUILTIN, OUTPUT); //dont need for i2c
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // dont need to blink led I was just
  delay(1000);                       //using to make sure arduino was active
  digitalWrite(LED_BUILTIN, LOW); 
  delay(1000);    
}
