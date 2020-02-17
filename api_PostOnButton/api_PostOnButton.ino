#define NET_ID "esp32"
#define PWD "netsoc"
#define SRC_URL "http://192.168.0.1:8080/"
#define deviceID "pick-A-Name"

#define rButtonPin 35
#define lButtonPin 0
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#include "SPI.h"
#include "TFT_eSPI.h"
TFT_eSPI tft = TFT_eSPI();

WiFiMulti wifiMulti;

String buttonPress(){
  while((digitalRead(lButtonPin) && digitalRead(rButtonPin))){
    delay(10);
  }
  
  if(!digitalRead(lButtonPin)){
    while(!digitalRead(lButtonPin)){
      delay(10);
    }
    return "lButton";
  }

  if(!digitalRead(rButtonPin)){
    while(!digitalRead(rButtonPin)){
      delay(10);
    }
    return "rButton";
  }
  return "balls";
  
}
String urlBase = SRC_URL;
void handleButton(){
    HTTPClient http;
    String tmpUrl = urlBase;
    tmpUrl.concat(buttonPress());
    Serial.println(tmpUrl);
    http.begin(tmpUrl);
    int httpCode = http.GET();
//    Serial.println(httpCode);
    http.end();

    if(httpCode > 0) {
        if(httpCode == HTTP_CODE_OK) {
            String getUrl = SRC_URL;
            getUrl.concat("showPosts?id=");
            getUrl.concat(deviceID);
            http.begin(getUrl);
            int httpRes = http.GET();
            if(httpRes == HTTP_CODE_OK){
                tft.fillScreen(0);
                tft.setCursor(0, 40);
                tft.print(http.getString());
            }else{
                tft.fillScreen(0);
                tft.setCursor(0, 40);
                tft.print("ERROR POSTING TO SERVER");             
            }
        }else{
            tft.fillScreen(0);
            tft.setCursor(0, 40);
            tft.print("ERROR POSTING TO SERVER");
        }
      
    }else{
        Serial.printf("[HTTP] error: %s\n", http.errorToString(httpCode).c_str());
        tft.fillScreen(0);
        tft.setCursor(0, 40);
        tft.print("NO SERVER CONNECTION");
    }
}


void setup() {

    Serial.begin(115200);
    
    wifiMulti.addAP(NET_ID);

    urlBase.concat("sendText");
    urlBase.concat("?");
    String id = "id=";
    id.concat(deviceID);
    String btn = "&string=";
    urlBase.concat(id);
    urlBase.concat(btn);

    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(0);
    pinMode(lButtonPin, INPUT_PULLUP);
    pinMode(rButtonPin, INPUT_PULLUP);
    delay(400);
        
}

void loop() {
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {
//        tft.fillScreen(0);
        handleButton();
    }else{
      tft.fillScreen(0);
      tft.setCursor(0, 40);
      tft.print("NO NETWORK CONNECTION");
    }
}
