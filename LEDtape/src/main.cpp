#include <FastLED.h>
#include<M5Atom.h>
#include<WiFi.h>
#include<WiFiUdp.h>

#define NUM_LEDS 10  //LEDの個数
#define DATA_PIN 22  //LED制御に使用するGPIOピン
#define COLOR_ORDER GRB //Green (G), Red (R), Blue (B)
#define CHIPSET WS2812B
#define BRIGHTNESS 60
#define VOLTS 5
#define MAX_AMPS 500 //value in milliamps
#define BUFSIZE 1024

int mode[4][3]={{255,228,206},{255,165,0},{255,0,0},{0,0,0}};//それぞれのモードのRGBを二次元配列で管理　 [0: 集中   1: リラックス  2: 警告   3:照明なし]
const char* ssid = "SPWH_L13_44E02B"; //WifiのSSID
const char* password = "3sL44wR6";    //パスワード
const int port = 5000;                //ポート番号
int data,click,num,past_num;          //クリック入力取得用

#define DELAYVAL 100 //LEDのピクセル間でのdelay

WiFiUDP udp;

CRGB leds[NUM_LEDS];



/////////////////////////// 三角波で輝度を変化させる関数/////////////////////////////////

void LED_bright_dark(int past, int now){

  for(int i=0;i<NUM_LEDS;i++)leds[i] = CRGB(mode[past][0],mode[past][1] ,mode[past][2]);
  for(int i=BRIGHTNESS;i>0;i--){ 
    FastLED.setBrightness(i);
    FastLED.show();
    delay(20); 

  }

  for(int i=0;i<NUM_LEDS;i++)leds[i] = CRGB(mode[now][0],mode[now][1] ,mode[now][2]);
  for(int i=0;i<BRIGHTNESS;i++){ 
    FastLED.setBrightness(i);
    FastLED.show();
    delay(20);

  }

}

//////////////////////////////////////////////////////////////////////////////////////


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



void loop() { 

M5.update();


//////////////M5Atomの内部ボタンのクリック入力取得/////////////////
if(M5.Btn.isPressed())data++;
  else data=0;

  if(data==1)click=1;
  else click=0;


  if(click==1){
    past_num=num;
    num++;
    LED_bright_dark(past_num, num);
  }

  if(num==4) num=0;

////////////////////////////////////////////////////////////

char packetBuffer[255];
    int packetSize = udp.parsePacket();
    
    if (packetSize) {
        udp.read(packetBuffer, packetSize);
        packetBuffer[packetSize] = 0;
    }

///////////////////LED の制御/////////////////////////////////

  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(mode[num][0],mode[num][1] ,mode[num][2]);
    FastLED.show();
    delay(10); //even shorter delay this time
  }

}