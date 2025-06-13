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
int brightness = 5;
int fadeAmount = 5;
int hue = 5;
int color = random(255);
int keepColor[NUM_LEDS] = {};
int origin = 0;
int wavelength = 10;
int direction = 1;
int fixedColor = random(255);
int invert = 0;
int invertBrightness = 205;
int count = 0;
int fadeAmountE = 5;
int defaultBrightness = 155;
int count_ST = 0; //生徒用ボタンのカウント(BがHighの時使う)

//Buttonチャタリング防止用
// bool lastButtonState = HIGH;  //直前のボタンのそのままの読み取り結果
// bool buttonState = HIGH;      //チャタリング防止後の確定したボタンの状態
const unsigned long debounceDelay = 50;

//先生_生徒用ボタンの値読み取り
bool button_State_ST = HIGH;
bool button_State_TH_A = HIGH;
bool button_State_TH_C = HIGH;

//ModeD用
int count_D = 0;

void modeA() {
  for(int i = 0;i < NUM_LEDS;i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void modeB() {
  if(brightness == 0 || brightness == defaultBrightness) {
    if(brightness == 0 && fadeAmount < 0){
    fadeAmount *= -1;
    color = random(255);
    }
    if(brightness == defaultBrightness && fadeAmount > 0){
    fadeAmount *= -1;
    }
  }

  for(int i = 0;i < NUM_LEDS;i++) {
    leds[i] = CHSV(color, 255, brightness);
  }

  brightness += fadeAmount;
  FastLED.show();
}

void modeC() {
  if(brightness == 0 || brightness == defaultBrightness) {
    if(brightness == 0 && fadeAmount < 0)
    fadeAmount *= -1;
    if(brightness == defaultBrightness && fadeAmount > 0)
    fadeAmount *= -1;
    if(brightness == 0){
    for(int i = 0;i < NUM_LEDS;i++) {
      int j = random(255);
      leds[i] = CHSV(j, 255, brightness);
      keepColor[i] = j;
      }
    }
  }

  for(int i = 0;i < NUM_LEDS;i++){
    leds[i] = CHSV(keepColor[i], 255, brightness);
  }
  brightness += fadeAmount;
  FastLED.show();
}


void modeD() {

  if(origin >= NUM_LEDS){
    direction = -1;
    fixedColor = random(255);
  }else if(origin <= -wavelength){
    direction = 1;
    fixedColor = random(255);
  }
  for(int i = 0;i < NUM_LEDS;i++) {
    if(i >= origin && i <= origin + wavelength)
    leds[i] = CHSV(fixedColor,255,defaultBrightness);
    else {
    leds[i] = CHSV(1,1,0);
    }
  }
  if(++count_D % 2 == 0){
    origin += direction;
  }
  
  //ここにdelay入れたら遅くなる?
  FastLED.show();

}

void modeE() {
  int groupNum = 5;
  if(brightness == 0 || brightness == defaultBrightness) {
    if(brightness == 0 && fadeAmountE < 0){
    fadeAmountE *= -1;
    color = random(255);
    count = (count + 1) % 2;
    }
    if(brightness == defaultBrightness && fadeAmountE > 0){
    fadeAmountE *= -1;
    }
  }

  for(int i = 0;i < NUM_LEDS;i++) {
    if((i % 10) >= 5){
      leds[i] = CHSV(color, 255, count * brightness);
    }
    else if((i % 10) < 5){
    leds[i] = CHSV(color, 255, (1 - count) * brightness);
    }
  }

  brightness += fadeAmountE;
  FastLED.show();
}





bool checkButtonPress(bool buttonState,int buttonPin) {
  unsigned long lastDebounceTime = 0;
  bool buttonPressed = false;
  int reading = digitalRead(buttonPin);

  //チャタリング防止
  if (reading != buttonState) {
    lastDebounceTime = millis();  
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {  //前回のボタン押下時からの経過時間がdebounceDelay以上だったら…
    if (reading != buttonState) {  //現在のボタンの状態が、前回記録されたボタンの状態と異なったら…
      buttonState = reading;
      if (buttonState == LOW) {  //ボタンが押されたら -> pullUPスイッチのため"=LOW"
        buttonPressed = true;
      }
    }
  }

  buttonState = reading;
  return buttonPressed;
}


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, WS2812B_PIN>(leds, NUM_LEDS);
  pinMode(BUTTON_PIN_ST, INPUT_PULLUP);
  pinMode(BUTTON_PIN_TH_A,INPUT_PULLUP);
  pinMode(BUTTON_PIN_TH_B,INPUT_PULLUP);
  for(int i = 0;i < NUM_LEDS;i++) {
    keepColor[i] = random(255);
  }

  modeA();
}

void loop() {
  if (checkButtonPress(button_State_TH_A, BUTTON_PIN_TH_A)) { //先生用ボタンAが押され→mode変更
    currentMode = (currentMode + 1) % 5;
    brightness = 0;
    Serial.print("Mode changed to:");
    Serial.println(currentMode);
  }

  if(digitalRead(BUTTON_PIN_TH_B)) { //先生用ボタンBがHIGHかLOWか検出
    if(checkButtonPress(button_State_ST,BUTTON_PIN_ST)) {     // 生徒用Cボタンの処理(BがHIGHの時)
      count_ST++;
      if(count_ST % 2 == 1){
      switch (currentMode) {
        case 0:
          modeB();
          delay(50);
          break;
        case 1:
          modeC();
          delay(50);
          break;
        case 2:
          modeD();
          delay(50);
          break;
        case 3:
          modeE();
          delay(50);
          break;
      }
      }else{
        modeA(); // 生徒用Cボタンが押されていない時は全LEDを消灯
      }

    }
  }else{
    if(digitalRead(BUTTON_PIN_ST)){// 生徒用Cボタンの処理(BがLOWの時)
       switch (currentMode) {
        case 0:
          modeB();
          delay(50);
          break;
        case 1:
          modeC();
          delay(50);
          break;
        case 2:
          modeD();
          delay(50);
          break;
        case 3:
          modeE();
          delay(50);
          break;
      }
    }else{
      modeA(); // 生徒用Cボタンが押されていない時は全LEDを消灯
    }
  }



}
