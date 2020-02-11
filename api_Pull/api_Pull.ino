#define NET_ID "esp32"
#define PWD "netsoc"
#define SRC_URL "http://192.168.0.1:8080/"

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#include "SPI.h"
#include "TFT_eSPI.h"
TFT_eSPI tft = TFT_eSPI();

WiFiMulti wifiMulti;

void setup() {

    Serial.begin(115200);
    
    wifiMulti.addAP(NET_ID, PWD);

    tft.begin();
    tft.setRotation(1);
}

void loop() {
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        
        http.begin(SRC_URL);
        
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been sent and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
                tft.fillScreen(0);
                tft.setCursor(0, 40);
                tft.print(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            tft.fillScreen(0);
            tft.setCursor(0, 40);
            tft.print("NO SERVER CONNECTION");
        }

        http.end();
    }else{
      tft.fillScreen(0);
      tft.setCursor(0, 40);
      tft.print("NO NETWORK CONNECTION");
    }

    delay(5000);
}
