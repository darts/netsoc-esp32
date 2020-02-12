#include <TFT_eSPI.h>
#include <SPI.h>
//#include <Arduino.h>

TFT_eSPI tft = TFT_eSPI();

#define DELAY_TIME 500
#define BODY_DIST 10 //distance between body blocks
#define CUBE_SIZE 8 

#define snakeBodyColour 0xD687
#define snakeHeadColour 0x3BF7
#define targetColour 0xC825
#define backgroundColour 0

#define rButtonPin 35
#define lButtonPin 0

int startLength = 10; //number of starting blocks
int curLength = startLength;
const int maxLength = 100; //maximum number of blocks
int xLocArr[maxLength];
int yLocArr[maxLength];

int dir = 0; //0 = up, 1 = right, 2 = down, 3 = left

void resetLocs(){
  for(int i =0; i < startLength; i++){
    xLocArr[i] = i * BODY_DIST;
    yLocArr[i] = BODY_DIST;
  }
}

void drawSnake(){
  tft.fillScreen(backgroundColour);
  for(int i = 0; i < curLength; i++){
    if(i == curLength-1)
      tft.fillRect(xLocArr[i], yLocArr[i], CUBE_SIZE, CUBE_SIZE, snakeHeadColour);
    else
      tft.fillRect(xLocArr[i], yLocArr[i], CUBE_SIZE, CUBE_SIZE, snakeBodyColour);
  }
}

void moveSnake(){
  for(int i = 0; i< curLength-1; i++){
    xLocArr[i] = xLocArr[i+1];
    yLocArr[i] = yLocArr[i+1];
  }
  switch(dir){
    case 0:
      xLocArr[curLength - 1] += BODY_DIST;
      yLocArr[curLength - 1];
      break;
    case 1:
      xLocArr[curLength - 1];
      yLocArr[curLength - 1] += BODY_DIST;
      break;
    case 2:
      xLocArr[curLength - 1] -= BODY_DIST;
      yLocArr[curLength - 1];
      break;
    case 3:
      xLocArr[curLength - 1];
      yLocArr[curLength - 1] -= BODY_DIST;
      break;
  }
}

boolean moved = false;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR lButtonPress(){
  portENTER_CRITICAL_ISR(&mux);
  if(!moved){
    directionUpdate(2);
    moved = true;
  }
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR rButtonPress(){
  portENTER_CRITICAL_ISR(&mux);
  if(!moved){
    directionUpdate(1);
    moved = true;
  }
  portEXIT_CRITICAL_ISR(&mux);
}

void directionUpdate(int dirIn){
  switch(dirIn){
    case 0:
      break;
    case 1:
      dir += 1;
      if(dir > 3)
        dir = 0;
      break;
    case 2:
      dir -= 1;
      if(dir < 0)
        dir = 3;
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(lButtonPin, INPUT_PULLUP);
  pinMode(rButtonPin, INPUT_PULLUP);

  attachInterrupt(lButtonPin, lButtonPress, FALLING);
  attachInterrupt(rButtonPin, rButtonPress, FALLING);
  
  tft.init();
   tft.setRotation(3);
   tft.fillScreen(backgroundColour);
   resetLocs();
   drawSnake();
   delay(DELAY_TIME * 3);
}

void loop() {
  moveSnake();
  drawSnake();
  moved = false;
  delay(DELAY_TIME);

}
