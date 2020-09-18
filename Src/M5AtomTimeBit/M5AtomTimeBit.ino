/*Lib section*/
/*------------------------------*/
#include <ArduinoJson.h>
#include <WiFi.h>
#include <M5Atom.h>
#include <HTTPClient.h>
#include <ETH.h>

/*Extern section*/
/*------------------------------*/
extern const unsigned char image_all[302];

extern const char* wifiSsid;   
extern const char* wifiPassword;

extern const char* urnBase;
extern const char* urnUserId;
extern const char* urnHookKey;

extern const char* openReason;
extern const char* closeReason;

extern const char* urnStatus;
extern const char* urnOpen;
extern const char* urnClose;

extern const char* rootCa;

/*Var section*/
/*------------------------------*/

bool IMU6886Flag = false;               //disable IMU

char urlStatus[200];                    //URL to recive status
char urlOpen[200];                      //URL to start
char urlClose[200];                     //URL to close

const char* statusBtrx;                 //Current status (OPENED, CLOSED, PAUSED, EXPIRED, ERROR)
//TODO:rename StatWifi to FirstWiFiCheck or something.
bool StatWifi = false;                  //Just flag
uint8_t SystemTimeUpdate=40;            //Counter to update interval


/*Program section*/
/*------------------------------*/
void setup()
{
    M5.begin(true, false, true);                      //Enable M5 Matrix
    delay(100); 
                                        
    M5.dis.displaybuff((uint8_t *)image_all, -10, 0); //Show load icon
    delay(50);
    
    Serial.begin(115200);                             //Activate virtual COM (debug)
    delay(50);
      
    CreateURLs();                                     //Generate URLs from URI (see Setting.ino)

    WiFi.begin(wifiSsid, wifiPassword);               //Connect to the network
    delay(50);

    while (WiFi.status() != WL_CONNECTED) {           //Wait for the Wi-Fi to connect
      delay(500);
      Serial.print('.');
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
      if (SystemTimeUpdate>=40){                          //if  time from last update Bitrix status less then 10s (40*250=10 000 ms)
        checkBitrix();                                    // -update status
        SystemTimeUpdate=0;                               // -reset the counter
      }
    }

    M5.update();                                          //Update M5
    if (M5.Btn.wasPressed())                              //Button handler
      {   
        Serial.println("\nBtn.Pressed!");                 //Debug message
        setBitrix();                                      //Open or close work day
      }

    delay(250);                                           //delay 250 ms
    SystemTimeUpdate++;                                   //Time counter
    Serial.print(".");                                    //Debug message
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
void setBitrix()
{
    Serial.println("setBitrix:Start");
    StaticJsonDocument<200> filter;
    filter["result"]["STATUS"] = true;
    StaticJsonDocument<1024> jsonDocument;
    int httpCode=0;
    HTTPClient http;
  
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
      Serial.println("updateScreen:OPENED"); 
      M5.dis.displaybuff((uint8_t *)image_all, 0, 0);
    }
    if((strcmp(statusBtrx,"CLOSED")==0) || (strcmp(statusBtrx,"EXPIRED")==0)){
      Serial.println("updateScreen:CLOSED"); 
      M5.dis.displaybuff((uint8_t *)image_all, -5, 0);
    }
    if(strcmp(statusBtrx,"PAUSED")==0){
      Serial.println("updateScreen:PAUSED"); 
      M5.dis.displaybuff((uint8_t *)image_all, -15, 0);
    }
    if(strcmp(statusBtrx,"ERROR")==0){
      Serial.println("updateScreen:OPENED"); 
      M5.dis.displaybuff((uint8_t *)image_all, -10, 0);
    }
   M5.update();
}


/*Generate URLs from URI (see Setting.ino)
------------------------------*/
void CreateURLs(){
    strcpy(urlStatus,urnBase);
    strcat(urlStatus,urnUserId);
    strcat(urlStatus,urnHookKey);
    strcat(urlStatus,"timeman.status");
    Serial.print("urlStatus:");
    Serial.println(urlStatus);
  
    strcpy(urlOpen,urnBase);
    strcat(urlOpen,urnUserId);
    strcat(urlOpen,urnHookKey);
    strcat(urlOpen,"timeman.open");
    Serial.print("urlOpen:");
    Serial.println(urlOpen);
    
    strcpy(urlClose,urnBase);
    strcat(urlClose,urnUserId);
    strcat(urlClose,urnHookKey);
    strcat(urlClose,"timeman.close");
    Serial.print("urlClose:");
    Serial.println(urlClose);
}
