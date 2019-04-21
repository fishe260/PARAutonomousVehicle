/**********
   Subsystem: Location
   Description: Gets the Kart's location relative to where it started

   GPS Packet:
   Header (UBX_HEADER)
   Class of information sent (0x01, Navigation)
   ID of sending module (0x02)
   Length of payload (28 bytes)
   Payload (Lat, Long, height, heightMSL, hAcc, vAcc)
   Checksum (Verify that all data is correct)

   Checksum adds up all data in NAV_gpsPacket data.
   (NAV_gpsPacket = Naviagtion Positioning Latitude Longitude & Height)
 **********/
#define GPS_STATUS_PIN 18 //0 = All good 1 = No connection to base

const unsigned char UBX_HEADER[] = { 0xB5, 0x62 }; //delcare UBX_HEADER to be 0xB5 0x62 (ub in ascii)

struct NAV_gpsPacket {
  unsigned char cls; //Class (should be 0x01)
  unsigned char id; //Identification number of module (should be 0x02)
  unsigned short len; //Length of packet (bytes)
  unsigned long iTOW; //GPS Time of Week (ms)
  long lon; //Longitude (1E7 * degrees)
  long lat; //Latitude (1E7 * degrees)
  long height; //Height above ellipsoid (mm)
  long hMSL; //Height above mean sea level (mm)
  unsigned long hAcc; //Horizontal/Longitude Estimate (mm)
  unsigned long vAcc; //Vertical/Latitude Estimate (mm)
};
NAV_gpsPacket gpsPacket;
const byte gpsPayloadSize = sizeof(NAV_gpsPacket);

void initGPS() {
  pinMode(GPS_STATUS_PIN, INPUT);
  Serial1.begin(19200);
}

void getLocation() {
  static int packetIndex = 0;
  static unsigned char checksum[2] = {0xFF, 0xFF};
  
  byte gpsByte = 0;

  if ( Serial1.available() ) {
    gpsByte = Serial1.read();
    
    if ( packetIndex < 2 ) { //Header                                 
      if ( gpsByte == UBX_HEADER[packetIndex] )                    
        packetIndex++;                                       
      else                                            
        packetIndex = 0; //Recieved wrong header.                                     
    }
    else {                                            
      if ( (packetIndex - 2) < gpsPayloadSize ) //Packet                 
        ((unsigned char*)(&gpsPacket))[packetIndex - 2] = gpsByte;    

      packetIndex++;                                         

      if ( packetIndex == (gpsPayloadSize + 2) ) { //Checksum start             
        calcChecksum(checksum); //This method edits checksum array                       
      }
      else if ( packetIndex == (gpsPayloadSize + 3) ) { //Checksum done       
        if ( gpsByte != checksum[0] ) //Checksum failed                      
          packetIndex = 0;                                   
      }
      else { //Packet reset         
        packetIndex = 0;                                    
      }
    }
  }

  //Must be connected to base
  locationStatus = ( !digitalRead(GPS_STATUS_PIN));

  /*
    Serial.print("iTOW:");      Serial.print(gpsPacket.iTOW);  //display data
    Serial.print(" lat/lon: "); Serial.print(gpsPacket.lat / 10000000.0f); Serial.print(","); Serial.print(gpsPacket.lon / 10000000.0f);
    Serial.print(" height: ");  Serial.print(gpsPacket.height / 1000.0f);
    Serial.print(" hMSL: ");    Serial.print(gpsPacket.hMSL / 1000.0f);
    Serial.print(" hAcc: ");    Serial.print(gpsPacket.hAcc / 1000.0f);
    Serial.print(" vAcc: ");    Serial.print(gpsPacket.vAcc / 1000.0f);
    Serial.println();
  */
}

void calcChecksum(unsigned char* checksumPointer) {
  memset(checksumPointer, 0, 2); //Fill array with zeros
  for (int i = 0; i < gpsPayloadSize; i++) {
    checksumPointer[0] += ((unsigned char*)(&gpsPacket))[i];
    checksumPointer[1] += checksumPointer[0];
  }
}
