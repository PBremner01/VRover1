#include"NTRIPClient.h"
bool NTRIPClient::reqSrcTbl(char* host,int &port)
{
  if(!connect(host,port)){
      Serial.print("Cannot connect to ");
      Serial.println(host);
      return false;
  }/*p = String("GET ") + String("/") + String(" HTTP/1.0\r\n");
  p = p + String("User-Agent: NTRIP Enbeded\r\n");*/
  print(
      "GET / HTTP/1.0\r\n"
      "User-Agent: NTRIPClient for Arduino v1.0\r\n"
      );
  unsigned long timeout = millis();
  while (available() == 0) {
     if (millis() - timeout > 5000) {
        Serial.println("Client Timeout !");
        stop();
        return false;
     }
     delay(10);
  }
  char buffer[50];
  readLine(buffer,sizeof(buffer));
  if(strncmp((char*)buffer,"SOURCETABLE 200 OK",17))
  {
    Serial.print((char*)buffer);
    return false;
  }
  return true;
    
}

bool NTRIPClient::reqSrcTbl(String shost,int &port){
  int ilgth = shost.length();
  char* chost = new char[ilgth + 1];
  strcpy(chost, shost.c_str());
  return reqSrcTbl(chost, port);

 }

bool NTRIPClient::reqRaw(char* host,int &port,char* mntpnt,char* user,char* psw)
{
    if(!connect(host,port))return false;
    String p="GET /";
    String auth="";
    Serial.println("Request NTRIP");
    
    p = p + mntpnt + String(" HTTP/1.0\r\n"
        "User-Agent: NTRIPClient for Arduino v1.0\r\n"
    );
    
    if (strlen(user)==0) {
        p = p + String(
            "Accept: */*\r\n"
            "Connection: close\r\n"
            );
    }
    else {
        auth = base64::encode(String(user) + String(":") + psw);
        #ifdef Debug
        Serial.println(String(user) + String(":") + psw);
        #endif

        p = p + String("Authorization: Basic ");
        p = p + auth;
        p = p + String("\r\n");
    }
    p = p + String("\r\n");
    print(p);
    #ifdef Debug
    Serial.println(p);
    #endif
    unsigned long timeout = millis();
    while (available() == 0) {
        if (millis() - timeout > 20000) {
            Serial.println("Client Timeout !");
            return false;
        }
        delay(10);
    }
    char buffer[50];
    readLine(buffer,sizeof(buffer));
    if(strncmp((char*)buffer,"ICY 200 OK",10))
    {
      Serial.print((char*)buffer);
      return false;
    }
    return true;
}

 bool NTRIPClient::reqRaw(String s_host,int &port,String s_mntpnt,String s_user,String s_psw)      //request RAW data from Caster 
{   
  char* c_host = new char[s_host.length() + 1];
  strcpy(c_host, s_host.c_str());
  
  char* c_mntpnt = new char[s_mntpnt.length() + 1];
  strcpy(c_mntpnt, s_mntpnt.c_str());

   char* c_user = new char[s_user.length() + 1];
  strcpy(c_user, s_user.c_str());

  char* c_psw = new char[s_psw.length() + 1];
  strcpy(c_psw, s_psw.c_str());

  return reqRaw(c_host,port, c_mntpnt, c_user, c_psw);
}

bool NTRIPClient::reqRaw(String s_host,int &port,String s_mntpnt)
{
  char* c_host = new char[s_host.length() + 1];
  strcpy(c_host, s_host.c_str());
  
  char* c_mntpnt = new char[s_mntpnt.length() + 1];
  strcpy(c_mntpnt, s_mntpnt.c_str());

  char* c_blk = 0;

  return reqRaw(c_host, port,c_mntpnt,c_blk, c_blk); 
}

bool NTRIPClient::reqRaw(char* host,int &port,char* mntpnt)
{
  char* c_blk = 0;
    return reqRaw(host,port,mntpnt, c_blk, c_blk);
}

int NTRIPClient::readLine(char* _buffer,int size)
{
  int len = 0;
  while(available()) {
    _buffer[len] = read();
    len++;
    if(_buffer[len-1] == '\n' || len >= size) break;
  }
  _buffer[len]='\0';

  return len;
}

int NTRIPClient::readLine(String sbuffer,int size)
{
  int ilgth = sbuffer.length();
  char* cbuffer = new char[ilgth + 1];
  strcpy(cbuffer, sbuffer.c_str());
  return readLine(cbuffer, size);
}