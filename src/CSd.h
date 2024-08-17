
#ifndef ARDUINO_CSD_H
#define ARDUINO_CSD_H
#include <Arduino.h>
#include <SD.h>

/*
 * Connect the SD card to the following pins:
 *
 * SD Card |   ESP32
 *    CS(CMD)    5
 *    SCK(CLK)  18
 *    MOSI      23
 *    MISO      19
 *    VCC       
 *    GRD
 */
//// Start of SD Card Congig //////////////////
#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  5
// End of SD Car Config /////////////////////

class CSd  
{
  private:
    File dataFile;

	public:
		CSd();
		~CSd();
		bool initSDCard();
    	bool opendataFile();
    	bool logSDCard(double GPSLat, double GPSLon);	
		void closeSDFile();	
};
#endif
