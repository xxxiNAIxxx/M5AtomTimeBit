/*Lib section*/
/*------------------------------*/
#include <ArduinoJson.h>
#include <WiFi.h>
#include <M5Atom.h>
#include <HTTPClient.h>
#include <ETH.h>

/*Define section*/
/*------------------------------*/
//#define DEBUG                      

/*Extern section*/
/*------------------------------*/
extern const unsigned char image_all[302];
extern const int serialSpeed;
extern const uint8_t updateTimeInterval;
extern uint8_t brightness;

extern const char* wifiSsid;   
extern const char* wifiPassword;

extern const char* urnBase;

extern const char* openReason;
extern const char* closeReason;

extern const char* urnStatus;
extern const char* urnOpen;
extern const char* urnClose;
extern const char* urnPause;

extern const char* rootCa;

/*Var section*/
/*------------------------------*/

bool IMU6886Flag = false;               //disable IMU

char urlStatus[200];                    //URL to recive status
char urlOpen[200];                      //URL to start
char urlClose[200];                     //URL to close
char urlPause[200];                     //URL to pause

const char* statusBtrx;                 //Current status (OPENED, CLOSED, PAUSED, EXPIRED, ERROR)
//TODO:rename StatWifi to FirstWiFiCheck or something.
bool StatWifi = false;                  //Just flag
uint16_t SystemTimeUpdate=updateTimeInterval*4;            //Counter to update interval


/*Program section*/
/*------------------------------*/
void setup()
{
    M5.begin(true, false, true);                      //Enable M5 Matrix
    delay(100); 
    
    if (brightness>90){
      brightness=90;
      }
      
    M5.dis.setBrightness(brightness);                 //Set brightness
    delay(100);                                
    M5.dis.displaybuff((uint8_t *)image_all, -10, 0); //Show load icon
    delay(50);
    
    Serial.begin(serialSpeed);                        //Activate virtual COM (debug)
    delay(50);
          
    CreateURLs();                                     //Generate URLs from URI (see Setting.ino)

    WiFi.begin(wifiSsid, wifiPassword);               //Connect to the network
    delay(50);

    while (WiFi.status() != WL_CONNECTED) {           //Wait for the Wi-Fi to connect
      delay(500);
      #ifdef DEBUG
        Serial.print('.');
      #endif  
    }
      Serial.println('\n');                             //Just debug messages
      Serial.println("Connection established");  
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP());
      Serial.println('\n'); 
}

void loop()
{
    if ((WiFi.status() != WL_CONNECTED) && !StatWifi ){   //If network disconnect and that first check then 
      M5.dis.displaybuff((uint8_t *)image_all, -15, 0);   // -show load icon
      StatWifi=true;                                      // -cahnge flag
    }
    else {                                                //network connected
      StatWifi=false;                                     //
      if (SystemTimeUpdate>=(updateTimeInterval*4)){      //if  time from last update Bitrix status less then 10s (40*250=10 000 ms)
        checkBitrix();                                    // -update status
        SystemTimeUpdate=0;                               // -reset the counter
      }
    }

    M5.update();                                          //Update M5

   if (M5.Btn.wasReleasefor(15)){                         //IF
      if (M5.Btn.wasReleasefor(750)){                     //long press
          setBitrix(0);                                   // -set state open\close
        }
      else{                                               //short press
          setBitrix(1);                                   // -set state pause\open
        }
   }

    delay(250);                                           //delay 250 ms
    SystemTimeUpdate++;                                   //Time counter
    #ifdef DEBUG
      Serial.print(".");
      Serial.print(SystemTimeUpdate);
      Serial.print(" RSSI:");
      Serial.print(WiFi.RSSI());                                    
    #endif
}


/*Get current bitrix status
------------------------------*/
void checkBitrix()
{
    StaticJsonDocument<200> filter;
    filter["result"]["STATUS"] = true;
    StaticJsonDocument<1024> jsonDocument;
    
    HTTPClient http;
    http.begin(urlStatus, rootCa);                     
    int httpCode = http.GET();
    if (httpCode > 0) {                           
        String payload = http.getString();
        DeserializationError error = deserializeJson(jsonDocument, payload, DeserializationOption::Filter(filter));
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            }
        statusBtrx = jsonDocument["result"]["STATUS"];   
        }
    else {
        Serial.println("checkBitrix:Error on HTTP request");
        statusBtrx = "ERROR";
        }
    http.end();
    
    updateScreen();
}

/*Change bitrix status
------------------------------*/
void setBitrix(uint8_t setState)
{
    Serial.println("setBitrix:Start");
    StaticJsonDocument<200> filter;
    filter["result"]["STATUS"] = true;
    StaticJsonDocument<1024> jsonDocument;
    int httpCode=0;
    HTTPClient http;
    switch(setState){
      case 0:
        if(strcmp(statusBtrx,"OPENED")==0){
          Serial.println("setBitrix:Current state - OPENED, will be - CLOSED");
          http.begin(urlClose, rootCa);
          httpCode = http.POST(closeReason);
        };
        if((strcmp(statusBtrx,"CLOSED")==0) || (strcmp(statusBtrx,"PAUSED")==0)){
          Serial.println("setBitrix:Current state - CLOSED, will be - OPENED");
          http.begin(urlOpen, rootCa);
          httpCode = http.POST(openReason);
        };
        break;
      case 1:
        if(strcmp(statusBtrx,"OPENED")==0){
          Serial.println("setBitrix:Current state - OPENED, will be - PAUSED");
          http.begin(urlPause, rootCa);
          httpCode = http.POST("");
        };
        if(strcmp(statusBtrx,"PAUSED")==0){
          Serial.println("setBitrix:Current state - PAUSED, will be - OPENED");
          http.begin(urlOpen, rootCa);
          httpCode = http.POST(openReason);
        };
      break;
    }

   
    if (httpCode > 0) {                                 
          String payload = http.getString();
          DeserializationError error = deserializeJson(jsonDocument, payload, DeserializationOption::Filter(filter));
          if (error) {
              Serial.print("setBitrix:");
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.c_str());
              }
          }
      else {
          Serial.println("setBitrix:Error on HTTP request");
          statusBtrx = "ERROR";
          }
      http.end();
  
      checkBitrix();
}

/*Update display
------------------------------*/
void updateScreen(){
    if(strcmp(statusBtrx,"OPENED")==0){
      #ifdef DEBUG
        Serial.println("updateScreen:OPENED");
      #endif
      M5.dis.displaybuff((uint8_t *)image_all, 0, 0);
    }
    if((strcmp(statusBtrx,"CLOSED")==0) || (strcmp(statusBtrx,"EXPIRED")==0)){
      #ifdef DEBUG
        Serial.println("updateScreen:CLOSED");
      #endif 
      M5.dis.displaybuff((uint8_t *)image_all, -5, 0);
    }
    if(strcmp(statusBtrx,"PAUSED")==0){
      #ifdef DEBUG
        Serial.println("updateScreen:PAUSED"); 
      #endif
      M5.dis.displaybuff((uint8_t *)image_all, -15, 0);
    }
    if(strcmp(statusBtrx,"ERROR")==0){
      #ifdef DEBUG
        Serial.println("updateScreen:OPENED");
      #endif 
      M5.dis.displaybuff((uint8_t *)image_all, -10, 0);
    }
   M5.update();
}


/*Generate URLs from URI (see Setting.ino)
------------------------------*/
void CreateURLs(){
    strcpy(urlStatus,urnBase);                //base urn
    strcpy(urlOpen,urnBase);                  //copy basic urn to other var
    strcpy(urlClose,urnBase);
    strcpy(urlPause,urnBase);
    
    strcat(urlStatus,urnStatus);              //basic urn + command
    strcat(urlOpen,urnOpen);
    strcat(urlClose,urnClose);
    strcat(urlPause,urnPause);
}
