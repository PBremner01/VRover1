#ifndef WEBSOCKET_H
#define WEBSOCKET_H
#include <ArduinoJson.h>

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        const uint8_t size = JSON_OBJECT_SIZE(2);
        StaticJsonDocument<size> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());

            return;
        }

        const char *action = json["action"];        
        //Serial.print("handleWebSocketMessage:"); Serial.println(action);
        if (strcmp(action, "tgl_gpsbtn") == 0) {
            bool bSave = oGPS.GetGPSBtn();
            oGPS.SetGPSBtn(!bSave);
            //gpsbtn.on = !gpsbtn.on;
            // oSAP.SaveGPSData(&ws, &oGPS);
            // oSAP.UpdateClientsAll(&ws, &oSteer, &oGPS); 
        }
        if (strcmp(action, "tgl_initialize") == 0) {
            oGPS.ZeroXYCoord();
        }  

        if (strcmp(action, "updt_speed") == 0) {
          const char *speed = json["speed"];
          oSteer.SetSpeed((int)atol(speed));          
         }               
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      //Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      //Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket()
{

  ws.onEvent(onEvent);
  server.addHandler(&ws);
  while ( !ws.availableForWriteAll() ){
       Serial.println("initWebSocket: Waiting on ws...");
       delay(1000);
  }
   Serial.println("websocket.h; initWebSocket Success"); 
}

String processor(const String& var){
  //Serial.println(var);
  if(var == "STATE"){
    // if (ledState){
    if (oGPS.GetGPSBtn()){
      return "ON";
    }
    else{
      return "OFF";
    } 
  }
  return String();
}

// void Sleep(boolean p_sleep) {

//     if (p_sleep == true) {
//         esp_deep_sleep_start();
//     };
// }

#endif
