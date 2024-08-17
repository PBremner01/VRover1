#ifndef ARDUINO_WIFI_H
#define ARDUINO_WIFI_H

#include <Arduino.h>
#include "WiFi.h"


class CWiFi
{
    private:
        const char* wifiid = "Scruffy";
        const char* wifipswd = "Rig!rat87";
            
    public:
    
    CWiFi();
    ~CWiFi();
    bool connectWiFi();
    void displayIP_Address();
};
#endif