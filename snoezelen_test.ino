#include <FastLED.h>
#include <Arduino.h>

#define WS2812B_PIN 7
#define NUM_LEDS 300
#define LED_TYPE WS2812B
#define BUTTON_PIN_ST 9
#define BUTTON_PIN_TH_A 6
#define BUTTON_PIN_TH_B 4

CRGB leds[NUM_LEDS]; 

// unsigned long lastDebounceTime_TH_A = 0;
// unsigned long lastDebounceTime_ST_C = 0;


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
int count_ST = 0; //生徒用ボタンのカウント(BがHighの時使う)

//Buttonチャタリング防止用
// bool lastButtonState = HIGH;  //直前のボタンのそのままの読み取り結果
// bool buttonState = HIGH;      //チャタリング防止後の確定したボタンの状態
// const unsigned long debounceDelay = 50;

//先生_生徒用ボタンの値読み取り
bool buttonStateSt = HIGH;
bool buttonStateThA = HIGH;
bool buttonStateThC = HIGH;

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

// bool checkButtonPress(bool &buttonState,int buttonPin,unsigned long &lastDebounceTime) {
//   bool buttonPressed = false;
//   int reading = digitalRead(buttonPin);

//   //チャタリング防止
//   if (reading != buttonState) {
//     lastDebounceTime = millis();  
//   }

//   if ((millis() - lastDebounceTime) > debounceDelay) {  //前回のボタン押下時からの経過時間がdebounceDelay以上だったら…
//     if (reading != buttonState) {  //現在のボタンの状態が、前回記録されたボタンの状態と異なったら…
//       buttonState = reading;
//       if (buttonState == LOW) {  //ボタンが押されたら -> pullUPスイッチのため"=LOW"
//         buttonPressed = true;
//       }
//     }
//   }


//   return buttonPressed;
// }


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, WS2812B_PIN>(leds, NUM_LEDS);
  pinMode(BUTTON_PIN_ST, INPUT_PULLUP);
  pinMode(BUTTON_PIN_TH_A,INPUT_PULLUP);
  pinMode(BUTTON_PIN_TH_B,INPUT_PULLUP);
  modeBlack();
}

void loop() {
  // if (checkButtonPress(buttonStateThA, BUTTON_PIN_TH_A, lastDebounceTime_TH_A)) { //先生用ボタンAが押され→mode変更
  if(digitalRead(BUTTON_PIN_TH_A) == LOW) { //先生用ボタンAが押された時
    currentMode = (currentMode + 1) % 5;
    brightness = 0;
    Serial.print("Mode changed to:");
    Serial.println(currentMode);
  }

  if(digitalRead(BUTTON_PIN_TH_B)) { //先生用ボタンBがHIGHかLOWか検出
    // if(checkButtonPress(buttonStateSt,BUTTON_PIN_ST, lastDebounceTime_ST_C)) {     // 生徒用Cボタンの処理(BがHIGHの時)
    if(digitalRead(BUTTON_PIN_ST) == LOW){ // 生徒用Cボタンが押された時
      count_ST++;
      if(count_ST % 2 == 1){
      switch (currentMode) {
        case 0:
          modeMonochroFade();
          // delay(50);
          break;
        case 1:
          modeMultipleFade();
          // delay(50);
          break;
        case 2:
          modeWaveMove();
          // delay(50);
          break;
        case 3:
          modeZebraPattern();
          // delay(50);
          break;
      }
      }else{
        modeBlack(); // 生徒用Cボタンが押されていない時は全LEDを消灯
      }

    }
  }else{
    if(digitalRead(BUTTON_PIN_ST)==LOW){// 生徒用Cボタンの処理(BがLOWの時)
       switch (currentMode) {
        case 0:
          modeMonochroFade();
          // delay(50);
          break;
        case 1:
          modeMultipleFade();
          // delay(50);
          break;
        case 2:
          modeWaveMove();
          // delay(50);
          break;
        case 3:
          modeZebraPattern();
          // delay(50);
          break;
      }
    }else{
      modeBlack(); // 生徒用Cボタンが押されていない時は全LEDを消灯
    }
  }



}
