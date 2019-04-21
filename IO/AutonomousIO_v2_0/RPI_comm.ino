const byte numChars = 6;

//need to fix these
int actualSpeed = 0;
byte bitflag;

boolean newDataSteering = false;
boolean sendTelemetryReq = false;

char ThrottleReceivedChars[numChars];
char SteeringReceivedChars[numChars];
 

void recvWithStartEndMarkers() {
    static boolean recvInProgressSteering = false;
    
    static byte ndx = 0;    
    char rc;

    char startMarkerSteering = '{';
    char endMarkerSteering = '}';
    char startTelemetry = '?';
    
    while (Serial.available() > 0 && (newDataSteering == false || newDataSteering == false || sendTelemetryReq == false)) {
        rc = Serial.read();
        if (recvInProgressSteering == true) {
          if (rc != endMarkerSteering) {
                SteeringReceivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                SteeringReceivedChars[ndx] = '\0'; // terminate the string
                recvInProgressSteering = false;
                ndx = 0;
                newDataSteering = true;
            }
        }
        else if (rc == startMarkerSteering) {
          recvInProgressSteering = true;
        }

        else if (rc == startTelemetry) {
          sendTelemetryReq = true;
        }
}
}

void showNewData() {
    if (newDataSteering == true) {
      String OutputSteeringString = String(SteeringReceivedChars);
      desiredSteeringAngleRPI = OutputSteeringString.toInt();
      Serial.print("^"); //Need to print thiss
      newDataSteering = false;
    }

    if (sendTelemetryReq == true) {
        Serial.print('/');
        Serial.print(desiredSpeed);
        Serial.print(',');
        Serial.print(actualSpeed);
        Serial.print(',');
        Serial.print(desiredSteeringAngle);
        Serial.print(',');          
        Serial.print(actualSteeringAngle);
        Serial.print(',');          
        Serial.print(int(voltage));
        Serial.print(',');          
        Serial.print(int(current));
        Serial.print(',');          
        Serial.print(mode);
        Serial.print(',');          
        Serial.print(bitflag);
        Serial.print('~');
        sendTelemetryReq = false;
    }
}
/*
const byte numChars = 6;

//need to fix these
int actualSpeed = 0;
byte bitflag;

boolean newDataThrottle = false;
boolean newDataSteering = false;
boolean sendTelemetryReq = false;

char ThrottleReceivedChars[numChars];
char SteeringReceivedChars[numChars];
 

void recvWithStartEndMarkers() {
    static boolean recvInProgressThrottle = false;
    static boolean recvInProgressSteering = false;
    
    static byte ndx = 0;    
    char rc;

    char startMarkerThrottle = '<';
    char endMarkerThrottle = '>';
    char startMarkerSteering = '{';
    char endMarkerSteering = '}';
    char startTelemetry = '?';
    
    while (Serial.available() > 0 && (newDataSteering == false || newDataSteering == false || sendTelemetryReq == false)) {
        rc = Serial.read();

        if (recvInProgressThrottle == true) {
            if (rc != endMarkerThrottle) {
                ThrottleReceivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                ThrottleReceivedChars[ndx] = '\0'; // terminate the string
                recvInProgressThrottle = false;
                ndx = 0;
                newDataThrottle = true;
            }
        }

        if (recvInProgressSteering == true) {
          if (rc != endMarkerSteering) {
                SteeringReceivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                SteeringReceivedChars[ndx] = '\0'; // terminate the string
                recvInProgressSteering = false;
                ndx = 0;
                newDataSteering = true;
            }
        }
        
        else if (rc == startMarkerThrottle) {
          recvInProgressThrottle = true;
        }

        else if (rc == startMarkerSteering) {
          recvInProgressSteering = true;
        }

        else if (rc == startTelemetry) {
          sendTelemetryReq = true;
        }
}
}

void showNewData() {
    if (newDataThrottle == true) {
      String OutputThrottleString = String(ThrottleReceivedChars); 
      OutputThrottleRPI = OutputThrottleString.toInt();
      //Serial.print(OutputThrottleRPI);
      Serial.print("#"); //Need to print this
      newDataThrottle = false;
    }
    
    if (newDataSteering == true) {
      String OutputSteeringString = String(SteeringReceivedChars);
      desiredSteeringAngleRPI = OutputSteeringString.toInt();
      Serial.print("^"); //Need to print thiss
      newDataSteering = false;
    }

    if (sendTelemetryReq == true) {
        Serial.print('/');
        Serial.print(desiredSpeed);
        Serial.print(',');
        Serial.print(actualSpeed);
        Serial.print(',');
        Serial.print(desiredSteeringAngle);
        Serial.print(',');          
        Serial.print(actualSteeringAngle);
        Serial.print(',');          
        Serial.print(int(voltage));
        Serial.print(',');          
        Serial.print(int(current));
        Serial.print(',');          
        Serial.print(mode);
        Serial.print(',');          
        Serial.print(bitflag);
        Serial.print('~');
        sendTelemetryReq = false;
    }
} */
