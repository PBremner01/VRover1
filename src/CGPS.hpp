
#ifndef ARDUINO_CGPS_H
#define ARDUINO_CGPS_H
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h> 
#include <string.h>
#include "CSd.hpp"
#include "NTRIPClient.h"
#include <WiFi.h>
#include <WiFiUDP.h>
#include <ArduinoJson.h>
#include "HardwareSerial.h"
#include "CUtility.hpp"

/*
 * Connect the GPS  card to the simpleRTK2B following pins:
 *
 * simpleRTK2B      |   ESP32
 *    SDA                 21    N/A
 *    GRD                 GRD                                                                           Green
 *    TX                  16    RX1 ESP RX <- GPS TX   rxPin: the pin on which to receive serial data   Green
 *    RX                  17    TX1 ESP TX <- GPS RX   txPin: the pin on which to transmit serial data. Red
 *    IORef				  3.3V                                                                          Red
 *    VCC                 5V                                                                            Red
 *    SCL                 22    N/A
 */
// Using ESP32 UART2
#define RX1 16                      // gpsTX1 -> RX
#define TX1 17                      // gpsRX1 -> TX
#define GPS_Baud 38400
const unsigned char UBX_HEADER[] = { 0xB5, 0x62 };   //NAV_POSECEF header
const unsigned char GPS_HEADER[] = { 0x24, 0x47 };   // $G NMEA header	
const unsigned char cTerm[] = { 0x0D, 0x0A };        // NMEA message termination characters
const float fMeterToFeet = 3.28084;
// float fXOffset = 0.0; 				//-264687153;     
// const float fYOffset = 0;					//-446422139;    
struct NAV_POSECEF {  // High Precision Postition ECEF
  // header = 2 bytes                           Packet byte offset        Packet bytes
  unsigned long buff;     // byte offset = 4              4                     4     //unknow field, not listed in UBX spec
  unsigned long iTOW;     // 				              8                     4
  long ecefx;             // 				              12                    4
  long ecefy;             // 					          16                    4
  long ecefz;             // 				              20                    4
  unsigned long pAcc;     // 					          24                    4
};
const unsigned int ECEF_MESSAGE_LGTH = 36;  //from interface description + 6
const unsigned int GPS_MESSAGE_LGTH = 100;  

struct XYCoord {
float fX, fY;
};

class CGPS : public CUtility
{
	private:
		CUtility* p_util;
		// SoftwareSerial *p_GPSSerial;
		TinyGPSPlus o_gps;								  // TinyGPS++ object
		CSd oSd;										  //handle to CSd
		double GPSLat;                                    //Latitude from GPS
		double GPSLon;                                    //Longitude from GPS
		double GPSCourseDeg;                              //Azimuth direction 
		float GPSAlt;                                     //Altitude from GPS
		float fXOffset = 0.0; 					//-264687153;     
		float fYOffset = 0.0;					//-446422139;  
		uint32_t GPSstarttimemS;                          //time in mS when GPS is scanned fo fix
		uint32_t GPSendtimemS;                            //time in mS when GPS got a fix
		bool bSaveGPS = false;							  //are we saving to SD?
		String host = "rtgpsout.unavco.org";					//Ntrip server xxx.xxx.xxx.xxx
		int httpPort = 2101; 								//port 2101 is default port of NTRIP caster
		String mntpnt = "P530_RTCM3";						//Mount Point Name
		String user   = "pbremner";							//User Name
		String passwd = "Ah8QSa2f";							//Password		
		HardwareSerial*  p_uart;
		WiFiUDP* p_wifiUdp; 
		#define PACKET_SIZE 4096
		static const int target_port = 28784;   			//Target port60000;			 	
		static const int my_port = 60000;  				 	//Own port
        struct str_gpsbtn {
            // state variables
            bool    on;
        };
        str_gpsbtn    gpsbtn = {false};  	
		uint8_t cHDR[2];
		uint8_t sECEF[ECEF_MESSAGE_LGTH];
		uint8_t sMessage[GPS_MESSAGE_LGTH];
		int iLgth;	
		XYCoord sXY;
		unsigned int message_pos;	
	public:
		SoftwareSerial *p_GPSSerial;	
		NTRIPClient ntrip_c;	
		NAV_POSECEF posecef;

		CGPS(CSd oSd, SoftwareSerial* p_GPSSerial, 
			HardwareSerial* o_uart, WiFiUDP* oWifiUdp, AsyncWebSocket* p_ws);
		~CGPS();
		void gpsBegin();
		void gpsWaitFix();
		bool ZeroXYCoord();
		float getCurrentCourseDeg();
		float getCurrentLat();
		float getCurrentLon();
		float getXCoordinate();
		float getYCoordinate();
		float getCoordAccuracyEst();
		// void displayGPSData();
		void displayCoordinates();
		void displayHTMLData();
		void displayGPSfix();
		void updateGPS(bool bDisplayLatLong = false, bool bDisplayXY = false);
		bool saveLatLon(bool b_on);
   		double getGPSAzi();
		double getGPSLat();
		double getGPSLon();
		bool GetSaveToGPSFlag();
		XYCoord GetXYCoord(float fLat, float fLong);		
		bool GetGPSBtn();
    	void SetGPSBtn(bool btn);
		bool NTRIP_Connect();	
		bool NTRIP_Stream();
		void display_nmea();
		void display_UBX_ECEF();
		bool processUBX_ECEF();
		void serialstream_monitor();

	private:
		void calcChecksum(unsigned char* CK);
};

#endif

