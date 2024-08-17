#ifndef ARDUINO_SOFTACCESSPOINT_H
#define ARDUINO_SOFTACCESSPOINT_H

#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>
#include "CSteer.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

class CSoftAccessPoint 
{
    private:
        const char* sapid = "VRover_Wifi";
        const char* sappswd = "";  //"Scruff01";
        bool bInitialSpeed;
        int i_sliderspeed;
        int iRow;
        String sRowData[5];

    public:
    CSoftAccessPoint();
    ~CSoftAccessPoint();
    bool connectSAP();
    bool isConnected();
    void displayIP_Address();
    void UpdateHTMLData(AsyncWebSocket* ws, bool bGPSBtn, double dLat, double dLong, double dAzi, String sText );
    void UpdateHTMLSpeed(AsyncWebSocket* ws, int iSpeed);
    void UpdateSerialStream(AsyncWebSocket* ws, String sText);
    String SetSerialText(String sText);
    // bool DisplayData(AsyncWebSocket* ws, String sText);



};
#endif