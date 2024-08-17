#ifndef NTRIP_CLIENT
#define NTRIP_CLIENT

#include <WiFiClient.h>
#include <Arduino.h>
#include<base64.h>

class NTRIPClient : public WiFiClient{
  public :
  bool reqSrcTbl(char* host,int &port);   //request MountPoints List serviced the NTRIP Caster 
  bool reqSrcTbl(String shost,int &port);   //request MountPoints List serviced the NTRIP Caster 
  bool reqRaw(char* host,int &port,char* mntpnt,char* user,char* psw);      //request RAW data from Caster 
  bool reqRaw(String s_host,int &port,String s_mntpnt,String s_user,String s_psw);      //request RAW data
  bool reqRaw(String s_host,int &port,String s_mntpnt); //non user from Caster 
  bool reqRaw(char* host,int &port,char* mntpnt); //non user
  int readLine(char* buffer,int size);
  int readLine(String sbuffer,int size);
  
};

#endif
