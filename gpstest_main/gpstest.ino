/*
Comment out the existing i2c file and replace it with the contents of this file
additional global variables also need to be added 
I've named all the values to transmit data1 - data6 with global scope
you can assign the values of the gps data you want to send to these vairables 
and every second the rpi will request the new gps value then store in on a text file

****IF THE ARDUINO is power cycled and the rpi is not, it might be in the middle of a request
**** and throw an I/O excpetion error on the pi, and stop the program from running, so make 
**** if the arduino is cycled when the pi is already on to then power cycle the pi
**** The opposite is not true, if the pi is cycled and not the arduino nothing will happen 
* ** except a delay in data collection (PI -> MASTER, ARDUINO -> SLAVE)
*/


//Libraries
#include <Wire.h> //Arduino Library for I2C

//Global
#define SLAVE_ADDRESS 0x04 //Addresss of the Arduino

void data_bytes(int input_data, byte* data_array) {
  data_array[0] = (input_data >> 24) & 0xFF;
  data_array[1] = (input_data >> 16) & 0xFF;
  data_array[2] = (input_data >> 8) & 0xFF;
  data_array[3] = input_data & 0xFF;
}

void sendTo(void){ //current behavior, send 6 ints on the bus byte by byte when requested
  byte data_array1[4];
  data_bytes(data1, data_array1);
  byte data_array2[4];
  data_bytes(data2, data_array2);
  byte data_array3[4];
  data_bytes(data3, data_array3);
  byte data_array4[4];
  data_bytes(data4, data_array4);
  byte data_array5[4];
  data_bytes(data5, data_array5);
  byte data_array6[4];
  data_bytes(data6, data_array6);
  Wire1.write(data_array1, 4); //longitude
  Wire1.write(data_array2, 4); //latitude
  Wire1.write(data_array3, 4); //height
  Wire1.write(data_array4, 4); //hMSL
  Wire1.write(data_array5, 4); //hAcc
  Wire1.write(data_array6, 4); //vAcc
}

void receiveData(int byteCount) {
  i=0; //index for arrray of stored passed values
  while(0<Wire1.available()){ //reads while wire is available
    number[i] = Wire1.read(); //read value from the bus and store in Array[index] 
    i++; //increment index
  }
}
