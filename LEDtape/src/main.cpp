#include <M5Atom.h>
#include<WiFi.h>
#include<WiFiUdp.h>
#include <Adafruit_NeoPixel.h>  //NeoPixel SK6812制御用ライブラリ

#define PIN        22  // LEDテープ信号端子
#define NUMPIXELS  10  // LEDの数

#define BUFSIZE 1024


const char* ssid     = "SPWH_LI3_44E02B_5G";
const char* password = "3sL44wR6";
const int port = 60000;

WiFiUDP udp;


int concent[3]={255,228,206};     //集中するときのRGB
int relax[3] ={255,165,0};     //リラックスするときのRGB
int warning[3]={255,0,0};       //警告するときのRGB

bool _concent=true;             //照明変化に使用するフラグ
bool _relax=true;
bool _warning =true;

int _click=0;
int mode=0;


// NeoPixel初期設定
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// 初期設定
void setup() {
  M5.begin(true, false, false);  //Serial,POWER,LED
  pixels.begin();     //NeoPixel初期化
  pixels.clear();     //LED色指定クリア


}
// メイン
void loop() {

  M5.update();
  char packetBuffer[BUFSIZE];


// M5atom スイッチのクリック入力取得

  if(M5.Btn.isPressed())data++;
  else data=0;

  if(data==1)_click=1;
  else _click=0;


  if(_click==1){
    mode++;
  }

  if(mode==4) mode=0;
  Serial.println(mode);

////////////////////////////// LED の制御/////////////////////////////////////////////
///// 0:リラックスモード  1:集中モード　2:警告モード　3:消灯////////////////////////////////

  switch(mode){
    case 0:
    //集中→リラックス

      _concent=true;

      if(_relax){
        for(int j=255;j>0;j-=10){
          pixels.setBrightness(constrain(j,0,255));

           for(int i = NUMPIXELS - 1; i >= 0; i--) {    //LED番号59～0まで繰り返し
            pixels.setPixelColor(i, pixels.Color(concent[0], concent[1], concent[2]));       
          
            }
            pixels.show();  //LED色出力

            }

         for(int j=0;j<255;j+=10){
          pixels.setBrightness(constrain(j,0,255));
          for(int i = NUMPIXELS - 1; i >= 0; i--) {    //LED番号59～0まで繰り返し
          pixels.setPixelColor(i, pixels.Color(relax[0], relax[1], relax[2]));       
          
         }
         pixels.show();  //LED色出力
         delay(50);

        }

        _relax=false;

      }


      for(int i = NUMPIXELS - 1; i >= 0; i--) {    //LED番号59～0まで繰り返し
      pixels.setPixelColor(i, pixels.Color(relax[0], relax[1], relax[2])); //全LED色指定(r,g,b)
      }
      pixels.show();  //LED色出力
      delay(50);


    break;

/////////////////////////////////////////////////////////////////////////////////////////////////
  

    case 1: 

    //リラックス→集中

     _relax=true;


       if(_concent){
        for(int j=255;j>0;j-=10){
          pixels.setBrightness(constrain(j,0,255));

           for(int i = NUMPIXELS - 1; i >= 0; i--) {    //LED番号59～0まで繰り返し
            pixels.setPixelColor(i, pixels.Color(relax[0], relax[1], relax[2]));       
          
            }
            pixels.show();  //LED色出力
            delay(50);
            }

         for(int j=0;j<255;j+=10){
          pixels.setBrightness(constrain(j,0,255));

          for(int i = NUMPIXELS - 1; i >= 0; i--) {    //LED番号59～0まで繰り返し
          pixels.setPixelColor(i, pixels.Color(concent[0], concent[1], concent[2]));       
          
         }
         pixels.show();  //LED色出力
         delay(50);

        }

        _concent=false;
      }

      for(int i = NUMPIXELS - 1; i >= 0; i--) {    //LED番号59～0まで繰り返し
          pixels.setPixelColor(i, pixels.Color(concent[0], concent[1], concent[2]));       
          
         }
         pixels.show();  //LED色出力


      break;


//////////////////////////////////////////////////////////////////////////////////////////////

    case 2:

      pixels.setBrightness(255);
      for(int i = NUMPIXELS - 1; i >= 0; i--) {    //LED番号59～0まで繰り返し
          pixels.setPixelColor(i, pixels.Color(warning[0], warning[1], warning[2]));
         }
         pixels.show();  //LED色出力

    break;


//////////////////////////////////////////////////////////////////////////////////////////////

    case 3:

      pixels.setBrightness(0);
      for(int i = NUMPIXELS - 1; i >= 0; i--) {    //LED番号59～0まで繰り返し
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
         }
         pixels.show();  //LED色出力

    break;  
 }
 
}