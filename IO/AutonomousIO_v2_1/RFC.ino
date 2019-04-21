void getRFCdata() {
  if (ch[4] > 20) {
    commRadioStatus = true;
    desiredSpeed = ch[1];
    desiredSteeringAngle = ch[3];
    if (ch[4] > 1195 && ch[4] < 1205) {
      mode = 0;
    }
    else if (ch[4] > 1295 && ch[4] < 1305) {
      mode = 1;
    }
  }
  else commRadioStatus = false; //If signal cuts out AT ALL   
}


void initRFC() {
  pinMode(RFCpin, INPUT);
  attachInterrupt(RFCpin, evalPPM, RISING);
  timeLast = micros();
}


void evalPPM() {
  unsigned long timeNew = micros();
  unsigned long timeDiff = timeNew - timeLast;
  timeLast = timeNew;
  if (timeDiff > 2500) {   // this must be the gap between set of pulses to synchronize
    valuesInt[NUM_CHANNELS] += timeDiff;
    RFCcounter = 0; 
    if (average == NUM_AVG) {
      for (int i = 0; i < NUM_CHANNELS + 1; i++) {
        ch[i] = (valuesInt[i] + 0.5) / average;
        valuesInt[i] = 0;
      }
      average = 0;
    }  
    average++;
  }
  else if (RFCcounter < NUM_CHANNELS) {
    valuesInt[RFCcounter] += timeDiff;
    RFCcounter++;
  }
}
