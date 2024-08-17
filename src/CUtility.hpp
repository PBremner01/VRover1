#ifndef ARDUINO_UTILITY_H
#define ARDUINO_UTILITY_H

#include <analogWrite.h>
#include <AsyncWebSocket.h>
#include "CSoftAccessPoint.hpp"

class CUtility : private CSoftAccessPoint
{
private:
    bool bInitial = false;

public:
    AsyncWebSocket* p_ws2;

    CUtility(AsyncWebSocket* p_ws1);
    ~CUtility();
    bool SetbInitial(bool bFlag);
    bool GetbInitial();
    bool DisplayHTMLData( bool bGPSBtn, double dLat, double dLong, double dAzi, String sText );
    bool DisplayHTMLSpeed( int iSpeed);
    bool DisplayData(String sText, int iNewLine = 0);
    bool DisplayData(String sText, float fData, int iNewLine = 0);
    bool DisplayData(String sText, int iData, int iNewLine = 0);
    void Wait(int nWait);


};

#endif