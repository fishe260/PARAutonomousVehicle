void initCompass() {
  mag.begin();
}

void readCompass() {
  sensors_event_t event;
  mag.getEvent(&event);
  heading = atan2(event.magnetic.y, event.magnetic.x);
  float declinationAngle = 0.074;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  headingDegrees = heading * 180/M_PI; 
}

