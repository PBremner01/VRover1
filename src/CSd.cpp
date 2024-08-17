#include "CSd.hpp"  

    CSd::CSd(){
    }

	CSd::~CSd(){
     }

    bool CSd::initSDCard(){
        Serial.println( "initSDCard");
        if(!SD.begin(CS)){
            Serial.println("SD Card Mount Failed");
        // don't do anything more:
            return false;
        }
        else {
            Serial.println("SD Card Mount succeeded.");
        }
        uint8_t cardType = SD.cardType();

        if(cardType == CARD_NONE){
            Serial.println("No SD card attached");
        }
        Serial.println(" --------------------");
        return true;
    }

    bool CSd::opendataFile()
    {
        if (!dataFile){
            SD.remove("/latlonData.txt");
            dataFile = SD.open("/latlonData.txt", FILE_WRITE);
            if (dataFile){
                Serial.println( "latlonData.txt file opened.");
                dataFile.println("Latitude  Longitude   Azi");     //add column headers
                return true;
            }
            else
            {
                Serial.println("opendataFile - failed latlonData.txt open");
                return false;
            }
            return true;
        }

        return false;
    }
 
    bool CSd::logSDCard(double GPSLat, double GPSLon, double GPSCourseDeg)
    {
     if ((GPSLat == 0.0)  || (GPSLon == 0.0) || (GPSCourseDeg == 0.0 ))
         return false;

        String s_data = "";
        String s_GPSLat = String(GPSLat, 8);
        String s_GPSLon = String(GPSLon, 8);
        String s_GPSCourseDeg = String(GPSCourseDeg, 2);
        String s_tab = "\t";
        s_data = s_GPSLat + s_tab + s_GPSLon + s_tab + s_GPSCourseDeg;
        if (dataFile){
            dataFile.println(s_data);
            Serial.println(s_data);
            return true;
        }
        else 
            return false;
    }

    void CSd::closeSDFile()
    {
        if (dataFile)
            dataFile.close();
    }
