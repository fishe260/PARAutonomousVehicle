/**********
   Subsystem: Safety Mechanisms
   Description: Checks statuses of subsystems and MC commands. Enacts contingencies for
   certain system failures.
   Upon any failure, program mode goes to 0 (disabled).
 *********/
void statusChecks() {
  //MC Commands
  if (MCCommands.remoteStop) {
    programMode = 0; //Disable the kart
  }

  //System status contingencies
  bitflag = CPUStatus | steeringStatus << 1 | throttleStatus << 2 | RFCStatus << 3 | positioningStatus << 4 | powerStatus << 5 | IMUStatus << 6 | telRadioStatus << 7;

  if (!RFCStatus) { //Bitflag protocols N$I
    programMode = 0; //Disable the kart
  }
  if (!telRadioStatus) {
  }
  if (!IMUStatus) {
    if (((programMode == 2 || programMode == 3) || programMode == 4) )
      programMode = 0; //Disable the kart if in auto mode
  }

  if (millis() - lastCPURecvTime > 500) {
    CPUStatus = false;
    if (((programMode == 2 || programMode == 3) || programMode == 4) )
      programMode = 0; //Disable the kart
  }
  else {
    CPUStatus = true;
  }

  //Disabled mode
  if ( (programMode == 0 || programMode == 5) || (programMode == 6 || programMode == 7) ) { //Disable kart
    desiredSteeringPosition = 128;
  }
}
