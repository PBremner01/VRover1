/*
 Name:		VineRover1.ino
 Created:	12/31/2021 3:51:10 PM
 Author: Phil Bremner	

*  To enable NMEA messages
 * Menu View -> Messages -> enable desired NMEA message (GGA, GNRMC)
 * Menu View -> Configuration View -> Prt 1-UART1 
 *                                          Baudrate matches below
 *                                          Protocol in 1 - NMEA
 * Menu View -> Generation 9 Advanced Configuration View -> CFG-MSGOUT
 *                                               ID_GGA_UART1 is enabled
 *                                               ID_RMC_UART1 is enabled
 *                                               CFG-UART1OUTPROT-NMEA is enabled
 * 
 *  To enable HPPOSECEF
 * Menu View -> Messages -> UBX-Nav
 *                                                HPPOSECEF is enabled (save changes)
 * Menu View -> Generation 9 Advanced Configuration View ->
 *                                               MSGOUT-UBX_NAV_POSECEF_UART1 is enabled
 * 
 */
#define debug_off false
#define LED_PIN_WIFI   32   // WiFi Status LED
#define ARDUINO_RUNNING_CORE 1

#define TFORWARD        1
#define TBACK           2
#define TRIGHT          3
#define TLEFT           4
// Enable esp32 sleep mode
#define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex
#define PIN_LED 27                                  // defined the LED pin 
#define Serial_Monitor_Baud 115200                  // Supports RemoteXY BLE

#include <Arduino.h>
#include "CSd.hpp"
#include <SoftwareSerial.h>
#include <ESPAsyncWebServer.h>
#include "CSoftAccessPoint.hpp"
#include "CGPS.hpp"
#include "CUtility.hpp"
#include "CSteer.hpp"
#include "CHC_SR04.hpp"
#include "index_html"
#include "CWifi.hpp"
#include "NTRIPClient.h"
#include <WiFi.h>
#include <WiFiUDP.h>
#include "HardwareSerial.h"
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
HardwareSerial oUart(2);                         //F9P Uart2
CSd oSd;
WiFiUDP oWifiUdp; 
CSteer oSteer;                               
CSoftAccessPoint oSAP;    
// SoftwareSerial oGPSSerial;                   
SoftwareSerial oGPSSerial(RX1, TX1);
CUtility oUtil(&ws);
CGPS oGPS(oSd, &oGPSSerial, &oUart, &oWifiUdp, &ws);
CHC_SR04 oSR04;
#include "websocket.h"
CWiFi oWiFi;

void NTRIP_Loop(void *pvParameters){
  while(1){
   if( oGPS.ntrip_c.available() ) {
        char ch = oGPS.ntrip_c.read();        
        oUart.write(ch);
    }
  }
}

void setup()
{
  Serial.begin(Serial_Monitor_Baud);
  delay(1000);

  // This lambda function initiates on event... regardless of being in setup module,
  // other modules in "setup" are not run.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("HTTP event server on.");
    int params = request->params();
    //Serial.print("Params: "); Serial.println(params); 
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter* p = request->getParam(i);
        //Serial.print("Param Name: "); Serial.print(p->name()); Serial.print(" ; "); Serial.println(p->value());

        //Controlling speed
        if (p->name() == "slider"){
            oSteer.SetSpeed(p->value().toInt());   
            // oSAP.SetSpeed(oSteer.GetSpeed());
        }   
        //Controlling direction buttons    
        if (p->name() == "TSTOP" ){ 
            oSteer.SetDirection('S');
            oSteer.Stop();
        }
        else if (p->name() == "TFORWARD" ){
            oSteer.SetDirection('F');
            oSteer.Forward(oSteer.GetSpeed(), oSR04);
        }
        else if (p->name() == "TBACK" ){
            oSteer.SetDirection('B');
            oSteer.Back(oSteer.GetSpeed(), oSR04);
        }
        else if (p->name() == "TLEFT" ){
            oSteer.SetDirection('F');
            oSteer.Left(oSteer.GetSpeed(), 30, oSR04);
        }
        else if (p->name() == "TRIGHT" ){
            oSteer.SetDirection('F');
            oSteer.Right(oSteer.GetSpeed(), 30, oSR04);  
        } 
    }

      request->send_P(200, "text/html", index_html, processor);
      if (!oUtil.GetbInitial()){
        oUtil.DisplayHTMLSpeed(60);                //initialize the slider speed and lable in Index.html
        oUtil.SetbInitial(true);
      }      
    
   });

  while (!oSAP.connectSAP()){
    Serial.println("Setup; oSAP.connectSAP failed.");
    delay(1000);
  }

  initWebSocket();
  server.begin();

  oSR04.initSR04Cards();

  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 1);   //Wake up on pin 36 on HIGH

  if (oWiFi.connectWiFi() == true){

    server.on("/hello", HTTP_GET, [](AsyncWebServerRequest * request) {
    
      // if (ON_STA_FILTER(request)) {
      //     request->send(200, "text/plain", "Hello from STA");
      //     return;
    
      // } else if (ON_AP_FILTER(request)) {
      //     request->send(200, "text/plain", "Hello from AP");
      //     return;
      // }

    });
  } 

  oGPS.gpsBegin();
  delay(10);
  // oGPS.NTRIP_Connect();
  // xTaskCreatePinnedToCore(NTRIP_Loop,"NTRIP_Loop",8192,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  oGPS.gpsWaitFix();
  oGPS.displayGPSfix();
}

void loop()
{
  //Serial.println("loop:");
  if ( 'S' == oSteer.GetDirection())
    oSteer.Stop();
  else if (oSR04.isStop(oSteer.GetDirection())){
    oSteer.Stop();
    Serial.println("main loop - force stop.");
  }

  oGPS.displayHTMLData();
  // oGPS.NTRIP_Stream(); 
  
  int iSpeed = oSteer.GetSpeed();
  oUtil.DisplayHTMLSpeed(oSteer.GetSpeed());      
  //Serial.print("iSpeed: "); Serial.println(iSpeed);          

  if( oGPS.GetSaveToGPSFlag())
    oGPS.saveLatLon(true);
 
  ws.cleanupClients(); 
 
  //For debug purpose
  // oGPS.serialstream_monitor();
  // if( oGPS.processUBX_ECEF() )
  //   Serial.println("processUBX true");
    
    // oGPS.display_nmea();       //Turn off ECEF message
    // oGPS.display_UBX_ECEF();      //Turn of NMEA messages

}