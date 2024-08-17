#include "CGPS.hpp"  

 CGPS::CGPS(CSd oSd, SoftwareSerial *p_GPSSerial, 
    HardwareSerial* o_uart, WiFiUDP* oWwifiUdp, AsyncWebSocket* p_ws) : CUtility(p_ws)
{
    this->oSd = oSd;
    this->p_GPSSerial = p_GPSSerial;
    p_uart = o_uart;
    p_wifiUdp = oWwifiUdp;
    str_gpsbtn    gpsbtn = {false};
    p_ws2 = p_ws;

    memset(sECEF, '\0', ECEF_MESSAGE_LGTH - 2);
    memset(sMessage, '\0', GPS_MESSAGE_LGTH);
    memset(cHDR, '\0', 2);
    message_pos = 0;    
}

CGPS::~CGPS()
{ }

void CGPS::gpsBegin()
{
    // p_GPSSerial->begin(GPS_Baud,SWSERIAL_8N1, RX1, TX1);            //We are using UART(2) for receiving of p_GPSSerial on RX1 (pin 16)
    p_GPSSerial->begin(GPS_Baud);                                   //We are using UART(2) for receiving of p_GPSSerial on RX1 (pin 16)
    p_uart->begin(GPS_Baud);                                        // and sending of the NTRIP data of p_uart(2) on TX1 (pin 17)
    p_wifiUdp->begin(my_port);
}

void CGPS::gpsWaitFix()
{
    // //waits till the GPS gets an updated fix
    uint8_t GPSchar;
    uint32_t i_runtime;
    GPSstarttimemS = millis();
    i_runtime = GPSstarttimemS + 1000;    // RemoteXY BLE has probs if >

    while (1)
    {
        if (millis() > i_runtime) 
            break;

        if (p_GPSSerial->available() > 0)
        {
            GPSchar = p_GPSSerial->read();
            o_gps.encode(GPSchar);
        }

        if (o_gps.location.isUpdated() && o_gps.altitude.isUpdated())
        {
            GPSendtimemS = millis();
            // while ( 1 ){                             //Synch ECEF data with GPS data
            //     if( processUBX_ECEF() )
            //         break;
            // }
            break;
        }
    };
}

bool CGPS::ZeroXYCoord()
{
    fXOffset = posecef.ecefx;
    fYOffset = posecef.ecefy;

    // XYCoord sXY = GetXYCoord( (float)GPSLat, (float)GPSLon );
    // fXOffset =sXY.fX ;
    // fYOffset = sXY.fY;    
    return true;
}

float CGPS::getCurrentCourseDeg() {
    float fDeg = (float)o_gps.course.deg();
     Serial.print( "GPS Course deg: "); Serial.print( fDeg ); 
     return fDeg;
}

float CGPS::getCurrentLat() {
    Serial.print(" lat: "); Serial.print(o_gps.location.lat(), DEC);
    return o_gps.location.lat();
}

float CGPS::getCurrentLon() {
    Serial.print(" lon: "); Serial.println(o_gps.location.lng(),DEC);
    return o_gps.location.lng();
}

float CGPS::getYCoordinate() {
    float fEcefy = ((posecef.ecefy - fYOffset) * fMeterToFeet)/100;
    //Serial.print(" Y: "); Serial.println(posecef.ecefy);
    return fEcefy;
}

float CGPS::getXCoordinate() {
    float fEcefx = ((posecef.ecefx - fXOffset) * fMeterToFeet)/100;
    //Serial.print(" X: "); Serial.println(posecef.ecefx);
    return fEcefx;
}

float CGPS::getCoordAccuracyEst() {
    float fAcc = (posecef.pAcc/100.0) * fMeterToFeet * 12;
    //Serial.print(" Acc: "); Serial.print(fAcc, DEC);
    return fAcc;
}

void CGPS::displayCoordinates(){
  //Serial.println("displayCoordinate(): ");
  bool bSinglePass = true;
    if ( bSinglePass && ( processUBX_ECEF() )) {
        String sDisplayTxt = "X(ft) :" + (String)getXCoordinate()
                      + "     Y(ft): " + (String)getYCoordinate() 
                      + "     Acc Est(in): " + (String)getCoordAccuracyEst();
        DisplayData(sDisplayTxt);
        bSinglePass = false;
    }
}

void CGPS::displayHTMLData(){
    //Serial.println("CGPS::displayHTMLData");
    String sDisplayTxt = "";
    XYCoord sXY;
    updateGPS(); 
            // while ( 1 ){                             //Synch ECEF data with GPS data
            //     if( processUBX_ECEF() )
            //         break;
            // }
    processUBX_ECEF();
    bool bSaveGPS = GetGPSBtn();

    double dLat = getGPSLat();
    double dLong = getGPSLon();
    double dAzi = getGPSAzi();
    // sXY = GetXYCoord(dLat, dLong);
    // sDisplayTxt = "X(ft) :"  + (String)(sXY.fX - fXOffset)
    //         + "     Y(ft): " + (String)(sXY.fY - fYOffset)
    //         + "     Acc Est(in): " + "0.0";
    sDisplayTxt = "X(ft) :" + (String)getXCoordinate() 
           + "     Y(ft): " + (String)getYCoordinate() 
           + "     Acc Est(in): " + (String)getCoordAccuracyEst();

    DisplayHTMLData( bSaveGPS, dLat, dLong, dAzi, sDisplayTxt);
    Serial.println(sDisplayTxt);

}

void CGPS::displayGPSfix()
{

    Serial.println();
    Serial.println();
    Serial.print(F("New GPS Fix "));
    Serial.print(GPSendtimemS - GPSstarttimemS);
    Serial.println(F("mS  "));

    GPSLat = o_gps.location.lat();
    GPSLon = o_gps.location.lng();
    GPSAlt = o_gps.altitude.meters();
    Serial.print(F("Lat,"));
    Serial.print(GPSLat, 6);
    Serial.print(F(",Lon,"));
    Serial.print(GPSLon, 6);
    Serial.print(F(",Alt,"));
    Serial.print(GPSAlt, 1);
    Serial.println();
}

void CGPS::updateGPS(bool bDisplayLatLong, bool bDisplayXY)
{
  XYCoord sXYCoord;

        gpsWaitFix();                                                 // Waiting on a verified GPS Update
        GPSCourseDeg = getCurrentCourseDeg();                         // Azimuth direction
        GPSLat = getCurrentLat();                                     //Latitude from GPS
        GPSLon = getCurrentLon();                                     //Longitude from GPS 
        sXYCoord= GetXYCoord(GPSLat, GPSLon);     

        if (bDisplayLatLong ){
          Serial.print(F("Lat:, "));
          Serial.print( GPSLat, 6);
          Serial.print(F(",Long:, "));
          Serial.println(GPSLon, 6);   
        } 

        if (bDisplayXY){
          Serial.print(F("XCoord:,"));
          Serial.print(sXYCoord.fX, 6);
          Serial.print(F(",YCoord:,"));
          Serial.println(sXYCoord.fY, 6); 
        }
}

bool CGPS::saveLatLon(bool b_on)
{
    bSaveGPS = b_on;
    if (b_on == true)
    {
        oSd.opendataFile();
        gpsWaitFix();
        GPSCourseDeg = getCurrentCourseDeg();                         // Azimuth direction
        GPSLat = getCurrentLat();                                     //Latitude from GPS
        GPSLon = getCurrentLon();                                     //Longitude from GPS  
        
        return  oSd.logSDCard(GPSLat, GPSLon, GPSCourseDeg);
    }
    else  //on exit close file
    {
         oSd.closeSDFile();
        return true;
    }
 
 }

double CGPS::getGPSAzi()
{
    return GPSCourseDeg;
};

double CGPS::getGPSLat()
{
    return GPSLat;
};

double CGPS::getGPSLon()
{
    return GPSLon;
};

bool CGPS::GetSaveToGPSFlag(){
    return bSaveGPS;   
};


bool CGPS::GetGPSBtn(){
    return gpsbtn.on;
}

XYCoord CGPS::GetXYCoord(float fLat, float fLong)
{
    float fEarthRadius = 6367 * 3.28084; 
    XYCoord sXYCoord;

    sXYCoord.fX = 0.0;
    sXYCoord.fY = 0.0;

    sXYCoord.fX = fEarthRadius * cos(fLat) * cos(fLong);
    sXYCoord.fY = fEarthRadius * cos(fLat) * sin(fLong);

    return sXYCoord;
}

void CGPS::SetGPSBtn(bool btn){
    gpsbtn.on = btn;
}

bool CGPS::NTRIP_Connect(){

    Serial.println("Requesting SourceTable.");
    if(ntrip_c.reqSrcTbl(host,httpPort)){
        char buffer[512];
        delay(5);
        while(ntrip_c.available()){
        ntrip_c.readLine(buffer,sizeof(buffer));
        Serial.print(buffer); 
        }
    }
    else{
        Serial.println("SourceTable request error");
    }
    Serial.print("Requesting SourceTable is OK\n");
    ntrip_c.stop(); //Need to call "stop" function for next request.
    
    Serial.println("Requesting MountPoint's Raw data");
    if(!ntrip_c.reqRaw(host,httpPort,mntpnt,user,passwd)){
        delay(15000);
        ESP.restart();
    }
    Serial.println("Requesting MountPoint is OK");  
    return true;
};

bool CGPS::NTRIP_Stream()
{
    IPAddress ip;
    IPAddress target_ip;
    byte buf1[PACKET_SIZE];
    byte buf2[PACKET_SIZE];
    int uart_len=0;

    while (!p_uart->available()){
        delay(100);
    }
    uart_len = p_uart->available();
    // Serial.print("NTRIP_Stream uart_len: "); Serial.println(uart_len);
    for(int i=0; i< uart_len; i++){
      buf2[i] = p_uart->read();                          // read char from UART
    }

    // Start UDP client fro sending packes   (https://github.com/espressif/arduino-esp32/issues/845)
    int connOK =  p_wifiUdp->begin(target_port);
    if (connOK == 0) {
        Serial.println("UDP could not get socket");
        return false;
    }
    int iBegin = p_wifiUdp->beginPacket(target_ip,target_port); 
   	if (iBegin == 0) { // Problem occured!
		p_wifiUdp->stop();
		Serial.println("UDP connection failed");
		return false;
	}
    int iWrite = p_wifiUdp->write(buf2,uart_len);
    if (iWrite == 0){
        p_wifiUdp->stop();
        Serial.println("CGPS::Ntrip_Stream; write failed.");
        return false;
    }
    int iEndPacket =  p_wifiUdp->endPacket();
    if (iEndPacket == 0){
        p_wifiUdp->stop();
        Serial.println("CGPS::Ntrip_Stream; endPacket failed.");
        return false;
    }

    Serial.print("..n ");
    return true;
}


void  CGPS::display_nmea(){
//Create a place to hold the incoming message
const unsigned int MAX_MESSAGE_LENGTH = 100;
static char message[MAX_MESSAGE_LENGTH];
static unsigned int message_pos = 0;
      
  //Check to see if anything is available in the serial receive buffer
  while (p_GPSSerial->available() > 0)
  {
    //Read the next available byte in the serial receive buffer
    char inByte = p_GPSSerial->read();
       
    //Message coming in (check not terminating character) and guard for over message size
    if( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1))
    {
    //Add the incoming byte to our message
    message[message_pos] = inByte;
    message_pos++;
    }
    //Full message received…
    else
    {
      //Add null character to string
      message[message_pos] = '\0';    
      //Print the message (or do other things)
      Serial.println(message);     
      //Reset for the next message
      message_pos = 0;
    }     //    if( inByte != '\n' &&
  }       //    while (p_GPSSerial->available() > 0)
}

void CGPS::calcChecksum(unsigned char* CK) {
  memset(CK, 0, 2);
  for (int i = 0; i < (int)sizeof(NAV_POSECEF); i++) {
    CK[0] += ((unsigned char*)(&posecef))[i];
    CK[1] += CK[0];
  }
}	

void CGPS::display_UBX_ECEF()
{
  if ( processUBX_ECEF() ) {
    Serial.print("iTOW:");      Serial.println(posecef.iTOW);
    Serial.print(" ecefx: ");  Serial.println(posecef.ecefx/100);
    Serial.print(" ecefy: ");  Serial.println(posecef.ecefy/100);
    Serial.print(" ecefz: ");  Serial.println(posecef.ecefz/100);    
    Serial.println();
  }
//   else
//     Serial.println("display_UBX_ECEF: processUBX_ECEF failed.");

}

bool CGPS::processUBX_ECEF(){
  //Serial.println("processUBC_ECEF()");
  // const unsigned int ECEF_MESSAGE_LGTH = 36;
  // message_pos = 0;
  static unsigned char checksum[2];
  // Serial.print("p_GPSSerial->available(): "); Serial.println(p_GPSSerial->available());

  //Go find the two ECEF message header bytes
  while ( p_GPSSerial->available() ) {
    // Serial.println("p_GPSSerial->available()");
    byte c = p_GPSSerial->read();
    if ( message_pos < 2 ) {
      if ( c == UBX_HEADER[message_pos] )
        message_pos++;
      else
        message_pos = 0;
    }
    else {
      if ( (message_pos) < ECEF_MESSAGE_LGTH )
        ((unsigned char*)(&posecef))[message_pos] = c;        

      message_pos++;

      if ( message_pos == (ECEF_MESSAGE_LGTH) ) {
        calcChecksum(checksum);
      }
      else if ( message_pos == (ECEF_MESSAGE_LGTH + 1) ) {
        if ( c != checksum[0] ){
          Serial.println("processGPS: checksum failed.");
          message_pos = 0;
        }

      }
      else if ( message_pos == (ECEF_MESSAGE_LGTH + 2) ) {
        message_pos = 0;
        if ( c == checksum[1] ) {
          return true;
        }
      }
      else if ( message_pos > (ECEF_MESSAGE_LGTH+4) ) {
        Serial.println("processGPS: payloadSize + 4 > message_pos");
        message_pos = 0;
      }
    }
  }
  return false;
}


void CGPS::serialstream_monitor(){
char cByte;
  
  //Check to see if anything is available in the serial receive buffer
  while (p_GPSSerial->available() )
  {  
    // Straight serial stream dump for debugging
                      cByte = p_GPSSerial->read();
                      Serial.print(cByte);    
  
    // if (message_pos == 0) {
    //   cByte = p_GPSSerial->read();
    //   cHDR[0] = (uint8_t)cByte;
    //   message_pos++;
    //   continue;
    // }

    // if (message_pos == 1){ 
    //   cByte = p_GPSSerial->read();
    //   cHDR[1] = (uint8_t)cByte;
    //    message_pos++;
    //   continue;
    // }      

    // if ( ( GPS_HEADER[0] == (unsigned char)cHDR[0] ) 
    //             && ( GPS_HEADER[1] == (unsigned char)cHDR[1] ) )
    // {
    //   if ( message_pos < 2 ) {
    //     continue;
    //   }
    //   else if ( message_pos <  GPS_MESSAGE_LGTH) {
    //     cByte = '\0';
    //     Serial.print((char)cHDR[0]);          //Header value
    //     Serial.print((char)cHDR[1]);          // Header value
    //     int iLgth = p_GPSSerial->readBytesUntil(GPS_HEADER[0], sMessage,  GPS_MESSAGE_LGTH);
    //     for ( int i = 0; i < iLgth; i++){
    //       Serial.print((char)sMessage[i]);
    //     }
        
    //     memset(cHDR,'\0', 2);  
    //     memset(sMessage,'\0',  GPS_MESSAGE_LGTH); 
    //     message_pos = 0;   
    //     p_GPSSerial->flush();  
    //     continue;
    //   }
    // }  
    // else if ( ( UBX_HEADER[0] == (unsigned char)cHDR[0] ) 
    //       && ( UBX_HEADER[1] == (unsigned char)cHDR[1] ) ) {

    //   if ( message_pos < 2 ) {
    //     continue;
    //   }
    //   else if ( message_pos < ECEF_MESSAGE_LGTH) {
    //     cByte = '\0';
    //     Serial.print((char)cHDR[0]);          //Header value
    //     Serial.print((char)cHDR[1]);          // Header value
    //     int iLgth = p_GPSSerial->readBytesUntil(UBX_HEADER[0], sECEF,  GPS_MESSAGE_LGTH);
    //     // sECEF[iLgth ] = cTerm[0];       // Add CR and LF line terminators
    //     // sECEF[iLgth + 1] = cTerm[1];        
    //     for ( int i = 0; i < iLgth; i++){
    //       Serial.print((char)sECEF[i]);
    //     }

    //     memset(cHDR,'\0', 2);  
    //     memset(sECEF,'\0', ECEF_MESSAGE_LGTH); 
    //     message_pos = 0;   
    //     p_GPSSerial->flush();     
    //     continue;
    //   } 
    // }
    // else
    // {
    //   message_pos = 0;
    //   memset(cHDR,'\0', 2);       
    //   continue;      //Synch on headers
    // } //   if ( ( UBX_HEADER[0] == (unsigned char)cHDR[0] ) 
  
  } //    while (p_GPSSerial->available() > 0)
}
