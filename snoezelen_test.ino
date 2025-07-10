#include <FastLED.h>
#include <Arduino.h>

#define WS2812B_PIN 7
#define NUM_LEDS 300
#define LED_TYPE WS2812B
#define BUTTON_PIN_ST 9
#define BUTTON_PIN_TH_A 6
#define BUTTON_PIN_TH_B 4

CRGB leds[NUM_LEDS]; 

int currentMode = 0;
int brightness = 0;
int fadeAmount = 5;
int color = random(255);
int setColor[NUM_LEDS] = {};
int origin = 0;
int wavelength = 10;
int direction = 1;
int invert = 0;
int invertBrightness = 205;
int controlZebraPattern = 0; //ゼブラパターンの制御(二つに分けたグループのうち,どちらを点灯させるか)
int maxBrightness = 155;
int count_ST = 0; //生徒用ボタンのカウント(BがHighの時使う)]

//modeAllRandom
int point;

//先生_生徒用ボタンの値読み取り
bool buttonStateSt = HIGH;
bool buttonStateThA = HIGH;
bool buttonStateThC = HIGH;
//生徒用ボタンC記憶用
int buttonStateStFlag = 0;
bool buttonLastStateSt = HIGH;
bool buttonLastStateThA = HIGH;
bool stateHolderST;
bool stateHolderThA;

//波の移動速度制御(秒数による制御無理だったのでカウントで制御)
int  delayWaveMove = 0;

void modeBlack() {
  for(int i = 0;i < NUM_LEDS;i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void modeMonochroFade() {
  if(brightness == 0 || brightness == maxBrightness) {
    if(brightness == 0 && fadeAmount < 0){
    fadeAmount *= -1;
    color = random(255);
    }
    if(brightness == maxBrightness && fadeAmount > 0){
    fadeAmount *= -1;
    }
  }

  for(int i = 0;i < NUM_LEDS;i++) {
    leds[i] = CHSV(color, 255, brightness);
  }

  brightness += fadeAmount;
  FastLED.show();
}

void modeMultipleFade() {
  if(brightness == 0 || brightness == maxBrightness) {
    if(brightness == 0 && fadeAmount < 0){
      fadeAmount *= -1;
      for(int i = 0;i < NUM_LEDS;i++) {
      setColor[i] = random(255);
      }
    }
    if(brightness == maxBrightness && fadeAmount > 0)
      fadeAmount *= -1;
  }

  for(int i = 0;i < NUM_LEDS;i++){
    leds[i] = CHSV(setColor[i], 255, brightness);
  }
  brightness += fadeAmount;
  FastLED.show();
}

void modeWaveMove() {

  if(origin >= NUM_LEDS){
    direction = -1;
    color = random(255);
  }else if(origin <= -wavelength){
    direction = 1;
    color = random(255);
  }
  for(int i = 0;i < NUM_LEDS;i++) {
    if(i >= origin && i <= origin + wavelength)
    leds[i] = CHSV(color,255,maxBrightness);
    else {
    leds[i] = CHSV(1,1,0);
    }
  }
  if(++ delayWaveMove % 3 == 0){
    origin += direction;
  }
  FastLED.show();

}

void modeZebraPattern() {
  int groupNum = 5;
  if(brightness == 0 || brightness == maxBrightness) {
    if(brightness == 0 && fadeAmount < 0){
    fadeAmount *= -1;
    color = random(255);
    controlZebraPattern =  (controlZebraPattern + 1) % 2;
    }
    if(brightness == maxBrightness && fadeAmount > 0){
    fadeAmount *= -1;
    }
  }

  for(int i = 0;i < NUM_LEDS;i++) {
    if((i % 10) >= 5){
      leds[i] = CHSV(color, 255, controlZebraPattern * brightness);
    }
    else if((i % 10) < 5){
      leds[i] = CHSV(color, 255, (1 - controlZebraPattern) * brightness);
    }
  }

  brightness += fadeAmount;
  FastLED.show();
}

void modeRed() {
  if(brightness == 0 || brightness == maxBrightness) {
    if(brightness == 0 && fadeAmount < 0){
    fadeAmount *= -1;
    }
    if(brightness == maxBrightness && fadeAmount > 0){
    fadeAmount *= -1;
    }
  }

  for(int i = 0;i < NUM_LEDS;i++) {
    //0→赤
    leds[i] = CHSV(0, 255, brightness);
  }

  brightness += fadeAmount;
  FastLED.show();
}

void modeAllRandom() {
  int groupNum = 5;
  if(brightness == 0 || brightness == maxBrightness) {
    if(brightness == 0 && fadeAmount < 0){
    fadeAmount *= -1;
    color = random(255);
    point = random(255-groupNum);
    }
    if(brightness == maxBrightness && fadeAmount > 0){
    fadeAmount *= -1;
    }
  }

  for(int i = 0;i < NUM_LEDS;i++) {
    if(i >= point && i < point+5){
      leds[i] = CHSV(color, point, brightness);
    }else{
      leds[i] = CRGB::Black;
    }
  }

  brightness += fadeAmount;
  FastLED.show();
}

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, WS2812B_PIN>(leds, NUM_LEDS);
  pinMode(BUTTON_PIN_ST, INPUT_PULLUP);
  pinMode(BUTTON_PIN_TH_A,INPUT_PULLUP);
  pinMode(BUTTON_PIN_TH_B,INPUT_PULLUP);
  modeBlack();
}

void loop() {

  stateHolderThA = digitalRead(BUTTON_PIN_TH_A);
  if(stateHolderThA == LOW && buttonLastStateThA == HIGH) { //先生用ボタンAが押された時
    currentMode = (currentMode + 1) % 6;
    if(currentMode == 1){
      brightness = 0;
      fadeAmount = -5;
    }
    brightness = 0;
    Serial.print("Mode changed to:");
    Serial.println(currentMode);
  }

  if(digitalRead(BUTTON_PIN_TH_B)) { //先生用ボタンBがHIGHかLOWか検出

    stateHolderST = digitalRead(BUTTON_PIN_ST);
    if(stateHolderST == LOW && buttonLastStateSt == HIGH){
      count_ST++;
    }
    if(count_ST % 2 == 1){ // 生徒用Cボタンが押された時   
      switch (currentMode) {
        case 0:
          modeMonochroFade();
          delay(50);
          break;
        case 1:
          modeMultipleFade();
          delay(50);
          break;
        case 2:
          modeWaveMove();
          delay(50);
          break;
        case 3:
          modeZebraPattern();
          delay(50);
          break;
        case 4:
          modeRed();
          delay(50);
          break;
        case 5:
          modeAllRandom();
          delay(50);
          break;
      }
    }else{
    modeBlack(); // 生徒用Cボタンが押されていない時は全LEDを消灯
    }
    buttonLastStateSt = stateHolderST;
  }else{
    if(digitalRead(BUTTON_PIN_ST)==LOW){// 生徒用Cボタンの処理(BがLOWの時)
       switch (currentMode) {
        case 0:
          modeMonochroFade();
          delay(50);
          break;
        case 1:
          modeMultipleFade();
          delay(50);
          break;
        case 2:
          modeWaveMove();
          delay(50);
          break;
        case 3:
          modeZebraPattern();
          delay(50);
          break;
        case 4:
          modeRed();
          delay(50);
          break;
        case 5:
          modeAllRandom();
          delay(50);
          break;
      }
    }else{
      modeBlack(); // 生徒用Cボタンが押されていない時は全LEDを消灯
    }
  }
  buttonLastStateThA = stateHolderThA;
}