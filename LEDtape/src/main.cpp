#include <FastLED.h>
#include<M5Atom.h>
#include<WiFi.h>
#include<WiFiUdp.h>

#define NUM_LEDS 10
#define DATA_PIN 22 //How boring and obvious!
#define COLOR_ORDER GRB //Green (G), Red (R), Blue (B)
#define CHIPSET WS2812B
#define BRIGHTNESS 60
#define VOLTS 5
#define MAX_AMPS 500 //value in milliamps
#define BUFSIZE 1024

int concent[3]={255,228,206};     //集中するときのRGB
int relax[3] ={255,165,0};     //リラックスするときのRGB
int warning[3]={255,0,0};       //警告するときのRGB

bool _concent=true;             //照明変化に使用するフラグ
bool _relax=true;
bool _warning =true;

int mode;                      //モードのデバッグ用

#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels


WiFiUDP udp;

const char* ssid = "SPWH_L13_44E02B";
const char* password = "3sL44wR6";
const int port = 5000;


CRGB leds[NUM_LEDS];

int data,click;

void setup() {

M5.begin(true, false, true);  //Serial,I2C,LED

FastLED.addLeds<CHIPSET,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MAX_AMPS);
FastLED.setBrightness(BRIGHTNESS);
FastLED.clear();
FastLED.show(); 


WiFi.config(IPAddress(192, 168, 0, 100), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 0, 1));
    WiFi.begin(ssid, password);
}

void loop() { //Swirly, twirly effect

M5.update();

////////// M5atom スイッチのクリック入力取得 ///////////////

if(M5.Btn.isPressed())data++;
  else data=0;

  if(data==1)click=1;
  else click=0;


  if(click==1){
    mode++;
  }

  if(mode==4) mode=0;


//////////////////////////////////////////////////////

char packetBuffer[255];
    int packetSize = udp.parsePacket();
    
    if (packetSize) {
        udp.read(packetBuffer, packetSize);
        packetBuffer[packetSize] = 0;

    }


Serial.print(mode);



////////////////////////////// LED の制御/////////////////////////////////////////////
///// 0:リラックスモード  1:集中モード　2:警告モード　3:消灯////////////////////////////////


switch(mode){

  case 0:// リラックス
    for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(255,165 ,30);
    FastLED.show();
    delay(10); 
  }
    break;

  case 1://集中
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(255,255 ,255);
    FastLED.show();
    delay(10); 
  }
    break;

  case 2://警告
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(255,0 ,0);
    FastLED.show();
    delay(10); 
  }

    break;

  case 3://無し
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(0,0 ,0);
    FastLED.show();
    delay(10); 
  }

    break;

}

}