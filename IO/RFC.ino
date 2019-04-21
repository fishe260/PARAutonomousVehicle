/**********
   Subsystem: RFC (Radio Frequency Control)
   Description: Remote control communication to IO
   PPM uses differing times between each pulse to communicate data.
   After one pulse for every channel there's a break (8-9 ms) between packets.
   Channel data is between 1000 (left/down) and 2000 (right/up) microseconds.
   Channel breakdown:
   [0] Right Stick L-R
   [1] Right Stick U-D
   [2] Left Stick U-D
   [3] Left Stick L-R
   [4] Mode Switch (1200, 1300, 1400, 1500, 1700, 1800)
   [5] SWA
   [6] VRA
   [7] VRB
   The trims on the remote affect the bias towards one side, aka what number the 'zero' position gives.
   Each trim click alters it by 4 microseconds (or about 1 on a 0-255 scale)

   Mode confirmation: After switching modes, hold throttle stick up for at least one second (trims @ 0) and then set back down.
 **********/
#define RFC_PIN 4
#define NUM_CHANNELS 8  //8-ch-Transmitter

volatile uint32_t lastTimeRFC, recvTimeRFC; //Used in PPM Evaluation
volatile uint16_t ch[NUM_CHANNELS]; //PPM values from remote-- range from 1000 to 2000
volatile byte channelCounter = NUM_CHANNELS; //Which channel we are on in packet construction

void initRFC() {
  pinMode(RFC_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RFC_PIN), evalPPM, RISING);

  channelCounter = 0;
  lastTimeRFC = 0;
  recvTimeRFC = false;
}

void evalPPM() {
  //This is the interrupt that runs whenever the RFC PPM pin goes high
  recvTimeRFC = micros();
  if (recvTimeRFC - lastTimeRFC > 2500) { //Actual gap between packets: ~8 or 9 ms
    channelCounter = 0;
  }
  else if (channelCounter < NUM_CHANNELS) {
    ch[channelCounter] = recvTimeRFC - lastTimeRFC;
    channelCounter++;
  }
  lastTimeRFC = recvTimeRFC;
}

void getRFCdata() {
  //Mode confirmation vars
  static bool sticksUpStart = false;
  static bool sticksUpComplete = false;
  static bool changeMode = false;
  static uint32_t sticksUpStartTime;
  static byte lastMode = 0; //Keeping track of lastModes

  RFCStatus  = true;

  //Translate the ch array to byte variables
  if (programMode != 7) {
    if (ch[4] > 1195 && ch[4] < 1205) { //RC
      programMode = 1;
      desiredThrottle = map(ch[1], 1000, 2000, 0, 255);
      desiredSteeringPosition = map(ch[3], 1000, 2000, 0, 255);
    }
    else if (ch[4] > 1295 && ch[4] < 1305) { //Train
      programMode = 2;
    }
    else if (ch[4] > 1395 && ch[4] < 1405) { //Time Trial
      programMode = 3;
    }
    else if (ch[4] > 1495 && ch[4] < 1505) { //Race
      programMode = 4;
    }
    else if (ch[4] > 1695 && ch[4] < 1705) { //UNUSED
      programMode = 5;
    }
    else if (ch[4] > 1795 && ch[4] < 1805) { //UNUSED
      programMode = 6;
    }
  }

  if ( ch[4] < 1000) { //If signal cuts out
    RFCStatus = false;
  }

  if ((lastMode != programMode) && (!changeMode && programMode != 0)) {
    programMode = 7;
  }
  else if (changeMode)
    changeMode = false;

  lastMode = programMode;

  //Check mode confirmation
  if ( (ch[1] > 1950 && !sticksUpStart) && programMode == 7) {
    sticksUpStartTime = millis();
    sticksUpStart = true;
  }

  if (sticksUpStart) {
    if (ch[1] < 1950 && !sticksUpComplete) {
      sticksUpStart = false;
    }

    if (millis() - sticksUpStartTime > 1000) {
      sticksUpComplete = true;
    }

    if (sticksUpComplete && ( (ch[0] == 1500 && ch[1] == 1500) && ch[3] == 1500 ) ) {
      programMode = 0;
      changeMode = true;
      sticksUpStart = false;
      sticksUpComplete = false;
    }
  }

  //Uncomment to view channel array
  /*
    Serial.print("Channel Array: ");
    for(int i = 0; i < NUM_CHANNELS; i++){
    Serial.print(ch[i]);
    Serial.print(", ");
    }
    Serial.println();
  */
}
