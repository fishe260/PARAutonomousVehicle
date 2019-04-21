public void serialEvent(Serial p) {
  String inString = p.readString();
  String data[] = splitTokens(inString, ",");
  //println(inString);
  throttle = int(data[0]); 
  actualSpeed = int(data[1]);
  desiredSteeringAngle = int(data[2]);
  actualSteeringAngle = int(data[3]);
  voltage = int(data[4]);
  current = int(data[5]);
  xAccel = int(data[6]);
  yAccel = int(data[7]);
  mode = int(data[8]);
  RFTPPS = int(data[9]);
  CPUIPS = int(data[10]);
  bitflag = byte(int( data[11].substring(0, data[11].length() - 2) ) );
  
  if(recToggle) {
    inString = year() + "-" + month() + "-" + day() + " " + hour() + ":" + minute() + ":" + second() + "," + inString;
    fileOutput.println(inString.substring(0, inString.length() - 1)); 
  }
}

//Mission Control Commands
public void sendData() { 
  if (RFTSerial.available() > 0) {
     RFTSerial.write((byte)(stopToggle ? 1 : 0));
  } 
}

public void captureEvent(Capture video) {
  video.read();
}
