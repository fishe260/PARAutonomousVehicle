void dialBlock() {
  numberOfDials = 8; //Brake, speed/throttle, Voltage, current, power, RF packets, CPU IPS
  dialSpacing = width/100;
  dialDiam = width/15; 
  leftX = width/2 - (numberOfDials*dialDiam + (numberOfDials-1)*dialSpacing)/2;
  centerDialY = height/50 + dialDiam/2;

  if(throttle < 128){
    throttleVal = 0;
    brakeVal = (int)(100*(128 - throttle) / 128);
  }  
  else {
    throttleVal = (int)(100*(throttle - 128) / 128);
    brakeVal = 0;
  }  
  n = 0;
  
  //DRAW BRAKE DIAL
  dial(leftX + n*dialDiam + n++*dialSpacing + dialDiam/2,centerDialY, brakeVal, 0 , 100 , "BRAKE", "%", 0);
  
  //DRAW THROTTLE DIAL
  dial(leftX + n*dialDiam + n++*dialSpacing + dialDiam/2,centerDialY, throttleVal, 0 , 100 , "THROTTLE", "%", 0);

  //DRAW SPEED DIAL
  dial(leftX + n*dialDiam + n++*dialSpacing + dialDiam/2,centerDialY,actualSpeed,0,maxSpeed,"SPEED", "MPH", 1);
  
  //DRAW VOLTAGE DIAL
  dial(leftX + n*dialDiam + n++*dialSpacing + dialDiam/2,centerDialY,voltage / 100.0, minVoltage, maxVoltage, "VOLTAGE", "V", 1);

  //DRAW CURRENT DIAL
  dial(leftX + n*dialDiam + n++*dialSpacing + dialDiam/2,centerDialY,current / 100.0,0,maxCurrent, "CURRENT", "A", 1);
  
  //DRAW POWER DIAL
  dial(leftX + n*dialDiam + n++*dialSpacing + dialDiam/2,centerDialY,power,0,maxPower, "POWER", "kW", 1);
  
  //DRAW RF PACKETS DIALS
  dial(leftX + n*dialDiam + n++*dialSpacing + dialDiam/2,centerDialY,RFTPPS,0,maxPPS, "RFT", "PPS", 0);
  
  //DRAW CPU SPEED DIALS
  dial(leftX + n*dialDiam + n++*dialSpacing + dialDiam/2,centerDialY,CPUIPS,0,maxCPUIPS, "CPU", "IPS", 0);
}  

void dial(int x, int y, float val, int minVal, int maxVal, String label, String unit, int decimals) {
  pushMatrix();
  fill(0);
  stroke(255);
  strokeWeight(height/1000);
  ellipse(x,y,dialDiam,dialDiam);
  strokeWeight(height/200);
  stroke(255,23,23);
  arc(x,y,dialDiam + height/200 - height/1000,dialDiam + height/200 - height/1000,PI,map(val,minVal,maxVal,0,TWO_PI)+PI);
  fill(255);
  textAlign(CENTER);
  textFont(fontLarge);
  if(decimals == 0)
      text(String.format("%.0f", val) ,x,y + smallSize/2 );
  else if(decimals == 1)
    text(String.format("%.1f", val) ,x,y + smallSize/2 );
  else if (decimals == 2)
      text(String.format("%.2f", val) ,x,y + smallSize/2 );
    
  textFont(fontSmall);
  text(unit, x , y + 2*dialDiam/5);
  text(label,x,y - dialDiam/5);
  popMatrix();
}

void statusIndicatorBlock() {
  indicatorDiameter = width/100;
  indicatorSpacing = largeSize * 3/2;
  indicatorStartX = 0; 
  indicatorStartY = height  - 8 * indicatorSpacing;
  
  pushMatrix();
  stroke(0,0,0);
  strokeWeight(height/1000);
  textAlign(CENTER);
  textFont(fontLarge);
  
  biggest = 0;
  for(String i : labels){
   if(i.length() > biggest)
     biggest = i.length();
  }
  
  fill(0);
  rect(indicatorStartX, indicatorStartY, indicatorStartX + (largeSize * 2/3 * biggest) + indicatorSpacing + indicatorDiameter , indicatorStartY + 8*indicatorSpacing);
  
  for(int n = 0 ; n < labels.length ; n++){ 
    fill(255);
    text(labels[n], indicatorStartX + (largeSize * 2/3 * biggest/2), indicatorStartY + largeSize/2 + n*indicatorSpacing + indicatorDiameter/2);
    fill(0,255,0); //GREEN = GOOD
    if( (bitflag >> n & 1) == 0)
      fill(255,0,0); //RED = BAD 
    ellipse(indicatorStartX + (largeSize * 2/3 * biggest) + indicatorSpacing , indicatorStartY + largeSize/2 + n*indicatorSpacing, indicatorDiameter,indicatorDiameter); 
  }
  fill(255);
  textFont(fontSmall);
  popMatrix(); 
}

void steerMeter() {
  rectLength = width/4;
  rectWidth = height/20;
  rectMeterX = int(0.5*width) - rectLength/2;
  rectMeterY = height - 2*rectWidth - wheelDiam;
  
  textAlign(CENTER);
  textFont(fontSmall);
  stroke(0);
  strokeWeight(height/1000);
  
  text("Desired Steering Angle", rectMeterX , rectMeterY - smallSize/2);
  rect(rectMeterX, rectMeterY, rectLength, rectWidth);
  text("Actual Steering Angle", rectMeterX , rectMeterY + rectWidth + smallSize/2 + smallSize/2);
  
  topAdjust = (int)map(desiredSteeringAngle, 0,255, rectMeterX , rectMeterX + rectLength);
  bottomAdjust = (int)map(actualSteeringAngle, 0,255, rectMeterX , rectMeterX + rectLength); 
 
  fill(0,255,0); 
  rect(topAdjust, rectMeterY,(rectMeterX + rectLength/2) - topAdjust ,rectWidth/2 );
  fill(255,0, 0);
  rect(bottomAdjust, rectMeterY + rectWidth/2, (rectMeterX + rectLength/2) - bottomAdjust ,rectWidth/2 );

  fill(255);
 
  pushMatrix();
  translate(width/2, rectMeterY - wheelDiam/2  );
  rotate(map(desiredSteeringAngle, 0,255,radians(minSteerAngle),radians(maxSteerAngle)));
  image(indicator, -wheelDiam/2,-wheelDiam/2);
  popMatrix();
  
  pushMatrix();
  translate(width/2, rectMeterY + rectWidth + wheelDiam/2  );
  rotate(map(actualSteeringAngle, 0,255,radians(minSteerAngle),radians(maxSteerAngle)));
  image(indicator, -wheelDiam/2,-wheelDiam/2);
  popMatrix();
}  

void textBlock() {
  //DRAW ENERGY USED TEXT
  textAlign(LEFT);
  textFont(fontSmall);
  
  string1 = "Energy Consumed: " + energyConsumed + " kWh";
  
  fill(0);
  rect(0, (float)1/4 * height- smallSize , "Mode: -No Remote Connection-".length() * smallSize * 2/3 , 3 * smallSize);
 
  fill(255);
  text(string1, width/200 , (float)1/4 * height);
 
  //DRAW MODE TEXT
  text("Mode: " + programModes[mode], width/200, (float)1/4 * height + smallSize);
}

void accelMeter() {
  fill(255);
  stroke(0);
  textFont(fontSmall);
  strokeWeight(height/400);
  ellipse(width - height/100 - imuDiam/2  , height - height/100 - imuDiam/2 , imuDiam, imuDiam); //Outer Circle
  fill(0);
  text(maxAccel + "G", width - height/100 - imuDiam/2 - 2*smallSize , height - imuDiam + smallSize );
  noFill();
  ellipse(width - height/100 - imuDiam/2  , height - height/100 - imuDiam/2 , imuDiam/2, imuDiam/2); //Inner Circle
  text(maxAccel/2 + "G", width - height/100 - imuDiam/2 - 2*smallSize,  height - imuDiam/2 - imuDiam/4 + smallSize );
  line(width - height/100 - imuDiam/2, height - height/100, width - height/100 - imuDiam/2, height - height/100 - imuDiam); //Vertical Line
  line(width - height/100, height - height/100 - imuDiam/2, width - height/100 - imuDiam, height - height/100 - imuDiam/2); //Horizontal Line
  fill(255,0,0, 150); //Indicator color
  //Indicator
  xAccel = map(xAccel, 0, 255, -IMUSensitivity, IMUSensitivity);
  yAccel = map(yAccel, 0, 255, -IMUSensitivity, IMUSensitivity);
  ellipse(map(xAccel, -maxAccel, maxAccel, width - height/100 - imuDiam , width - height/100) , map(yAccel, -maxAccel, maxAccel, height - height/100 , height - height/100 - imuDiam ), imuDiam/8, imuDiam/8);
}

void commands() {
  textAlign(CENTER);
  textFont(fontLarge);
  
  //RECORD BUTTON
  fill(255);
  strokeWeight(height/500);
  ellipse(height/100 + height/14 , indicatorStartY - stopDiam - height/100 - recDiam , recDiam, recDiam);
  if(!recToggle)
    fill(255,0,0);
  else
    fill(0);
  ellipse(height/100 + height/14 , indicatorStartY - stopDiam - height/100 - recDiam, recDiam/3, recDiam/3);

  //ESTOP BUTTON
  strokeWeight(height/300);
  stroke(0);
  if(!stopToggle) {
    fill(150,0,0);
    ellipse(height/100 + height/14 , indicatorStartY - stopDiam/2 - height/100 , stopDiam, stopDiam);
    fill(255);
    text("STOP", height/100 + stopDiam/2, indicatorStartY - stopDiam/2 - height/100 + largeSize * 1/3);
  }
  else {
    fill(0,150,0);
    ellipse(height/100 + height/14 , indicatorStartY - stopDiam/2 - height/100 , stopDiam, stopDiam);
    fill(255);
    text("RESUME", height/100 + stopDiam/2, indicatorStartY - stopDiam/2 - height/100 + largeSize * 1/3);
  }
}

void mousePressed() {
  if( pow(mouseX - (height/100 + height/14) ,2) + pow(mouseY - (indicatorStartY - stopDiam/2 - height/100), 2) < pow(stopDiam/2,2) ) { 
    stopToggle ^= true;
  }  
  else if( pow(mouseX - (height/100 + height/14) ,2) + pow(mouseY - (indicatorStartY - stopDiam - height/100 - recDiam), 2) < pow(recDiam/2,2) ) {
    recToggle ^= true;
  }
}  

void fps() {
  stroke(255);
  fill(255);
  textFont(fontSmall);
  textAlign(LEFT);

  text("FPS: " + FPSCounterLabel, 0, smallSize);
  
  FPSCounter++;
  
  if(millis() - lastFPSTime > 1000) {
    lastFPSTime = millis();
    FPSCounterLabel = FPSCounter;
    FPSCounter = 0;
  }
}
