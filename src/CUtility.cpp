#include "CUtility.hpp"

CUtility:: CUtility(AsyncWebSocket* p_ws1)
{
    p_ws2 = p_ws1;
    //Serial.println("CUtility()");
}
CUtility::~CUtility(){}

bool CUtility::DisplayHTMLData( bool bGPSBtn, double dLat, double dLong, double dAzi, String sText ){

    this->UpdateHTMLData(p_ws2, bGPSBtn, dLat, dLong, dAzi, sText);
    return true;
}

bool CUtility::SetbInitial(bool bFlag){
    bInitial = bFlag;
    return bInitial;
}

bool CUtility::GetbInitial(){
    return bInitial;
}

bool CUtility::DisplayHTMLSpeed( int iSpeed){

    UpdateHTMLSpeed(p_ws2, iSpeed);
    return true;
}

bool CUtility::DisplayData(String sText, int iNewLine)
{   //Serial.println("CUtility::DisplayData(String sText, int iNewLine)");

    //if (iNewLine) sText+= '\n';

    Serial.println(sText);
    UpdateSerialStream( p_ws2, sText);
     return true;  
}

bool CUtility::DisplayData(String sText, float fData, int iNewLine){

    sText += (String)(fData);
    // if (iNewLine) sText+= "/n";
    Serial.println(sText);
    UpdateSerialStream( p_ws2, sText);
    return true;  
}

bool CUtility::DisplayData(String sText, int iData, int iNewLine){

    sText += (String)iData;
    //if (iNewLine) sText+= "/n";
    Serial.print(sText);
    UpdateSerialStream( p_ws2, sText);
    return true;  
}

void CUtility::Wait(int nWait){
    int nMillisec = millis();
    int nCurrentDelay = 0;

    while (nCurrentDelay < nWait)
    {
        nCurrentDelay = millis() - nMillisec;
    }

}
