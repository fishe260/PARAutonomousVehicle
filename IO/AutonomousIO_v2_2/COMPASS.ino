/*

bool detectHMC5883L () {
  // read identification registers
  Wire.beginTransmission(HMC5883L_ADDR); //open communication with HMC5883
  Wire.write(10); //select Identification register A
  Wire.endTransmission();
  Wire.requestFrom(HMC5883L_ADDR, 3);
  if(3 == Wire.available()) {
    char a = Wire.read();
    char b = Wire.read();
    char c = Wire.read();
    if(a == 'H' && b == '4' && c == '3')
      return true;
  }

  return false;

  flushI2C();
}


void readCompass() { 
  int16_t x,y,z; //triple axis data

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(HMC5883L_ADDR);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(HMC5883L_ADDR, 6);
  if(6 <= Wire.available()){
    x = Wire.read()<<8|Wire.read();   //X msb|lsb
    z = Wire.read()<<8|Wire.read();   //Z msb|lsb
    y = Wire.read()<<8|Wire.read();   //Y msb|lsb
  }
  
  //heading = atan2((double)y,(double)x) * (180 / 3.14159265) + 180; // angle in degrees  
  heading = atan2((double)y,(double)x);
  heading += .074;
  if (heading < 0) heading += 2*PI;

  if (heading > 2*PI) heading -= 2*PI;

  heading *= (180/PI);
  flushI2C();
}


void initCompass() {
  Wire.begin();
  Wire.setClock(400000);
  detect = detectHMC5883L();
  Wire.beginTransmission(HMC5883L_ADDR); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x01); //continuous measurement mode
  Wire.endTransmission();
}
*/
