#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

#define DELAY_TIME 500
#define BODY_DIST 10 //distance between body blocks
#define CUBE_SIZE 8 
#define TGT_SIZE 8
#define DIFFICULTY 50

#define snakeBodyColour 0xD687
#define snakeHeadColour 0x3BF7
#define targetColour 0xC825
#define backgroundColour 0
#define deathColour 0xFFFF

#define rButtonPin 35
#define lButtonPin 0

#define DEBUGGING false
#define bounceTime 250
#define WIDTH 230
#define HEIGHT 120

int startLength = 11; //number of starting blocks
int curLength = startLength;
const int maxLength = 100; //maximum number of blocks
int xLocArr[maxLength];
int yLocArr[maxLength];

int speedModifier = 0;

int score = 0;

int dir = 0; //0 = up, 1 = right, 2 = down, 3 = left

void resetLocs(){
  #if DEBUGGING
    Serial.print("Spawning snake length=");
    Serial.println(startLength);
  #endif
  
  for(int i =1; i < startLength; i++){
    xLocArr[i] = i * BODY_DIST;
    yLocArr[i] = BODY_DIST;
  }
}

void clearScreen(){
  tft.fillScreen(backgroundColour);
}

void drawSnake(){
  for(int i = 1; i < curLength; i++){
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
  #if DEBUGGING
    Serial.print("Head x: ");
    Serial.print(xLocArr[curLength - 1]);
    Serial.print("     Head y: ");
    Serial.println(yLocArr[curLength - 1]);
  #endif
}

volatile uint32_t debounceCurrentL = 0;
volatile uint32_t debouncePreviousL = 0;
volatile uint32_t debounceCurrentR = 0;
volatile uint32_t debouncePreviousR = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR lButtonPress(){
  portENTER_CRITICAL_ISR(&mux);
  debounceCurrentL = xTaskGetTickCount();
  if(debounceCurrentL - debouncePreviousL > bounceTime){
    debouncePreviousL = debounceCurrentL;
    directionUpdate(2);
  }
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR rButtonPress(){
  portENTER_CRITICAL_ISR(&mux);
  debounceCurrentR = xTaskGetTickCount();
  if(debounceCurrentR - debouncePreviousR > bounceTime){
    debouncePreviousR = debounceCurrentR;
    directionUpdate(1);
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

void death(){
  #if DEBUGGING
    Serial.print("RIPerino... - Score:  "); 
    Serial.println(score);
  #endif 
  tft.setTextColor(deathColour);
  String resStr = "R.I.P - Score:  ";
  String scoreStr = String(score);
  resStr.concat(scoreStr);
tft.drawCentreString(resStr, 110, 30, 2);
delay(5000);
  ESP.restart();
}

boolean collideSnake(int x, int y, int notIncludeHead, boolean onlyHead = false){
  if(onlyHead){
    return (xLocArr[curLength-1] == x && yLocArr[curLength-1] == y);
  }
  
  for(int i = 1; i < curLength - notIncludeHead; i++){
    if(xLocArr[i] == x && yLocArr[i] == y)
      return true;
  }
  return false;
}

boolean checkSelfCollision(){
  return collideSnake(xLocArr[curLength - 1],yLocArr[curLength - 1], 1);
}

void checkBounds(){
  if(xLocArr[curLength - 1] > WIDTH || 
     xLocArr[curLength - 1] < 0 || 
     yLocArr[curLength - 1] > HEIGHT || 
     yLocArr[curLength - 1] < 0 ||
     checkSelfCollision())
    death();
}

int roundToBodyDist(int input){
  return (input / BODY_DIST) * BODY_DIST; 
}

int tgtX, tgtY;
boolean activeTgt = false;
void spawnTgt(){
  byte cur = 0;
  byte attempts = 8; //try to spawn this many times
  while(true){
    tgtX = roundToBodyDist(random(0, WIDTH));
    tgtY = roundToBodyDist(random(0, HEIGHT));
    if(!collideSnake(tgtX, tgtY, 0)){
      #if DEBUGGING
        Serial.print("Spawned tgt @ x="); 
        Serial.print(tgtX);
        Serial.print("  y="); 
        Serial.println(tgtY);
      #endif
      activeTgt = true;
      return;
    }
    cur++;
    if(cur > attempts)
      death();
  }
}
#if DEBUGGING
void printSnake(){
  for(int i = 0; i < curLength; i++){
    Serial.print(xLocArr[i]);
    Serial.print(" . ");
  }
  Serial.println("");
  
  for(int i = 0; i < curLength; i++){
    Serial.print(yLocArr[i]);
    Serial.print(" . ");
  }
  Serial.println("");
}
#endif

void drawTgt(){
  if(!activeTgt)
    spawnTgt();
  tft.fillRect(tgtX, tgtY, TGT_SIZE, TGT_SIZE, targetColour);
}

void addSnakeBody(){
  #if DEBUGGING
  printSnake();
  #endif
  curLength++;
  for(int i = curLength-1; i > 0 ; i--){
    xLocArr[i] = xLocArr[i-1];
    yLocArr[i] = yLocArr[i-1];
  }
  speedModifier++;
  #if DEBUGGING
  printSnake();
  #endif
}

void tgtCollideSnake(){
  if(collideSnake(tgtX, tgtY, 0, true)){
   #if DEBUGGING
      Serial.print("Snake hit tgt @ x=");
      Serial.print(tgtX);
      Serial.print("  y="); 
      Serial.println(tgtY);
   #endif
   score++;
   addSnakeBody();
   activeTgt = false;
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
   speedModifier = 0;
}

void loop() {
  clearScreen();
  drawTgt();
  moveSnake();
  drawSnake();
  checkBounds();
  tgtCollideSnake();
  
  delay(DELAY_TIME - (speedModifier*DIFFICULTY));

}
