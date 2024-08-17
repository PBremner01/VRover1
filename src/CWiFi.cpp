#include "CWiFi.hpp"    
    
    
    CWiFi::CWiFi(){};
    CWiFi::~CWiFi(){};

    bool CWiFi::connectWiFi(){
        WiFi.begin(wifiid, wifipswd);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi..");
        }
        Serial.print("ESP32 IP on the router WiFi network: ");
        Serial.println(WiFi.localIP());  

        return true;
    };

    void CWiFi::displayIP_Address(){

    };