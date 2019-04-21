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

/*
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
    Serial.print(posllh.CurrentLAT);
    Serial.print(',');
    Serial.print(posllh.CurrentLONG);
    Serial.print(',');
    Serial.print(heading);
    Serial.print(',');          
    Serial.print(mode);
    Serial.print(',');          
    Serial.print(bitflag);
    Serial.print('~');
 */   
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
            else if (rc == endMarkerThrottle) {
                ThrottleReceivedChars[ndx] = '\0'; // terminate the string
                recvInProgressThrottle = false;
                ndx = 0;
                newDataThrottle = true;
                String OutputThrottleString = String(ThrottleReceivedChars); 
                OutputThrottleRPI = OutputThrottleString.toInt();
                Serial.print("#"); //Need to print this
                newDataThrottle = false;
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
          else if (rc == endMarkerSteering) {
                SteeringReceivedChars[ndx] = '\0'; // terminate the string
                recvInProgressSteering = false;
                ndx = 0;
                newDataSteering = true;
                String OutputSteeringString = String(SteeringReceivedChars);
                desiredSteeringAngleRPI = OutputSteeringString.toInt();
                Serial.print("^"); //Need to print thiss
                newDataSteering = false;
            }
        }
        
        else if (rc == startMarkerThrottle) {
          recvInProgressThrottle = true;
        }

        else if (rc == startMarkerSteering) {
          recvInProgressSteering = true;
        }
        else if (rc == startTelemetry) {
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
          Serial.print(posllh.CurrentLAT);
          Serial.print(',');
          Serial.print(posllh.CurrentLONG);
          Serial.print(',');
          Serial.print(heading);
          Serial.print(',');          
          Serial.print(mode);
          Serial.print(',');          
          Serial.print(bitflag);
          Serial.print('~');
        }
    }
}

