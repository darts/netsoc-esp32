#include <TFT_eSPI.h>
#include "SPI.h"
#include <WiFi.h>
#include <WebServer.h>

#define NET_ID "esp32"
#define PWD "netsoc"

const char* ssid     = NET_ID;
const char* password = PWD;

WebServer server(80);

TFT_eSPI tft = TFT_eSPI();

String theWebpage = "<html><a href=/buttonA><button>A Button</button></a><a href=/buttonB><button>B Button</button></a></html>";

void handleRoot() {  
  server.send(200, "text/html", theWebpage);
}

void handleClickA(){
  server.send(200, "text/html", theWebpage);
  tft.fillScreen(120);
}

void handleClickB(){
  server.send(200, "text/html", theWebpage);
  tft.fillScreen(10612809);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  tft.setCursor(0, 40);
  tft.print("File Not Found");
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(0);
    
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.setCursor(0, 40);
    tft.print("NO NETWORK CONNECTION");
  }
  tft.fillScreen(0);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/buttonA", handleClickA);
  server.on("/buttonB", handleClickB);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
