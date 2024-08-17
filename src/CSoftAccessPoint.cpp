#include "CSoftAccessPoint.hpp"

// template sort functions from:    https://github.com/emilv/ArduinoSort/tree/master/examples/SortArray
	template<typename AnyType> bool builtinLargerThan(AnyType first, AnyType second) {
		return first > second;
	}

	template<> bool builtinLargerThan(char* first, char* second) {
		return strcmp(first, second) > 0;
	}

	template<typename AnyType> void insertionSort(AnyType array[], size_t sizeOfArray, bool reverse, bool (*largerThan)(AnyType, AnyType)) {
		for (size_t i = 1; i < sizeOfArray; i++) {
			for (size_t j = i; j > 0 && (largerThan(array[j-1], array[j]) != reverse); j--) {
				AnyType tmp = array[j-1];
				array[j-1] = array[j];
				array[j] = tmp;
			}
		}
	}

    template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray) {
	insertionSort(array, sizeOfArray, true, builtinLargerThan);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


CSoftAccessPoint::CSoftAccessPoint(){
    iRow = 0;
};

CSoftAccessPoint::~CSoftAccessPoint(){};

bool CSoftAccessPoint::connectSAP()
{
    WiFi.mode(WIFI_AP);                       //Set Soft Access Point mode; added 02/24/2022
    WiFi.softAP(sapid, sappswd);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Soft Access Point WiFi AP IP address: ");
    Serial.println(IP);

    if ( IP )
        return true;
    else
        return false;
    
};

bool CSoftAccessPoint::isConnected(){
    return WiFi.isConnected();
}

void CSoftAccessPoint::displayIP_Address(){
    
    IPAddress IP = WiFi.softAPIP();   
    Serial.print(IP);
};

String CSoftAccessPoint::SetSerialText(String sText){

    return sText;
}

void CSoftAccessPoint::UpdateHTMLData(AsyncWebSocket* ws, bool bGPSBtn, double dLat, double dLong, double dAzi, String sText ){
  //Serial.println("UpdateHTMLData");
    char txt_lat[13];
    char txt_lon[13];   
    char txt_azi[13];
    char data[500];                                        // data len is 95....
    char txt_ID_SS[100];
    const uint size = JSON_OBJECT_SIZE(200);
    StaticJsonDocument<size> json;
    
    if (bGPSBtn == true)
       json["status"] = "on";
    else
       json["status"] = "off";

    dtostrf(dLat, 0, 6, txt_lat); 
    json["latitude"] = txt_lat;                        
    dtostrf(dLong, 0, 6, txt_lon);    
    json["longitude"] = txt_lon;                         //{"longitude":"101"} => len = 20 + 1
    dtostrf(dAzi, 0, 3, txt_azi);    
    json["azimuth"] = txt_azi;

    if (!sText.isEmpty()){
        // sText = "Empty";
        sRowData[iRow] = (String)millis() + ": " + sText;
        sortArrayReverse(sRowData, 5);

        for (int j = 0; j < 5; j++){
            txt_ID_SS[0] = 0;



            strcpy(txt_ID_SS, sRowData[j].c_str()); 

            switch (j){
                case 0:  
                    json["ID_SS0"] = txt_ID_SS;
                    break;
                case 1:
                    json["ID_SS1"] = txt_ID_SS;
                    break;
                case 2:
                    json["ID_SS2"] = txt_ID_SS;
                    break;
                case 3:
                    json["ID_SS3"] = txt_ID_SS;
                    break;
                case 4:
                    json["ID_SS4"] = txt_ID_SS;
                    break;
                default:
                    json["ID_SS1"] = txt_ID_SS;
                    Serial.print("Error in CSoftAccessPoint::UpdateSerialString  Row#: "); Serial.println(iRow);
                    break;
            }
        }
            iRow++;
            if ( iRow > 4)
                iRow = 0;

    }  //if (!sText.isEmpty()){

    size_t len = serializeJson(json, data);
    ws->textAll(data, len);
}

void CSoftAccessPoint::UpdateHTMLSpeed(AsyncWebSocket* ws, int iSpeed)
{
  //Serial.println("UpdateHTMLSpeed");

    char txt_speedval[15];
    char data[20];                                        // data len is 17...
    const uint size = JSON_OBJECT_SIZE(20);
    StaticJsonDocument<size> json;
    String sInt = String(iSpeed);

    strcpy(txt_speedval, sInt.c_str());
    json["speedval"] = txt_speedval;                 

    size_t len = serializeJson(json, data);
    ws->textAll(data, len);
}

void CSoftAccessPoint::UpdateSerialStream(AsyncWebSocket* ws, String sText)
{   //Serial.println("CsoftAccerssPoint::UpdateSerialStream");

    if ( sText.length() == 0 ) return;

    try{
        char data[3000];
        char txt_ID_SS[100];
        const uint size = JSON_OBJECT_SIZE(50);
        StaticJsonDocument<size> json;

        for (int j = 0; j < 5; j++){
            txt_ID_SS[0] = 0;
            if (iRow == j)
                sRowData[iRow] = (String)millis() + ": " + sText;

            if (sRowData[j].isEmpty())
                sRowData[j] = "Empty";

            strcpy(txt_ID_SS, sRowData[j].c_str()); 

            switch (j){
                case 0:  
                    json["ID_SS0"] = txt_ID_SS;
                    break;
                case 1:
                    json["ID_SS1"] = txt_ID_SS;
                    break;
                case 2:
                    json["ID_SS2"] = txt_ID_SS;
                    break;
                case 3:
                    json["ID_SS3"] = txt_ID_SS;
                    break;
                case 4:
                    json["ID_SS4"] = txt_ID_SS;
                    break;
                default:
                    json["ID_SS1"] = txt_ID_SS;
                    Serial.print("Error in CSoftAccessPoint::UpdateSerialString  Row#: "); Serial.println(iRow);
                    break;
            }
        }
            iRow++;
            if ( iRow > 4)
                iRow = 0;
            size_t len = serializeJson(json, data);
            ws->textAll(data, len);

        } catch (const char* msg) {
            Serial.println("In UpdateSerialStream exception.");
        }

}

// void CSoftAccessPoint::SaveGPSData(AsyncWebSocket* ws, CGPS* gps){
//     const uint8_t size = JSON_OBJECT_SIZE(1);
//     StaticJsonDocument<size> json;
//     json["status"] = gpsbtn.on?"on":"off";
//     char data[22];
//     size_t len = serializeJson(json, data);
//     //Serial.print("SaveGPSData gpsbtn.on: "); Serial.println(gpsbtn.on); 
//     ws->textAll(data, len);
//     gpsbtn.on?gps->saveLatLon(true):gps->saveLatLon(false);    
// }

// bool CSoftAccessPoint::DisplayData(AsyncWebSocket* p_ws, String sText){

//     Serial.print(sText);
//     UpdateSerialStream( p_ws, sText);
//     return true;  
// }

