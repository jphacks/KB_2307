#include <FastLED.h>
#include<M5Atom.h>
#include<WiFi.h>
#include<WiFiUdp.h>

#define NUM_LEDS 10 //LEDの個数
#define DATA_PIN 22 //LED制御に使用するGPIOピン
#define COLOR_ORDER GRB //Green (G), Red (R), Blue (B)
#define CHIPSET WS2812B
#define BRIGHTNESS 60 //LEDの輝度
#define VOLTS 5
#define MAX_AMPS 500 //value in milliamps
#define BUFSIZE 1024
#define DELAYVAL 100 //LEDのピクセル間でのdelay

WiFiUDP udp;
CRGB leds[NUM_LEDS];

const char* ssid = "SPWH_L13_44E02B"; //WifiのSSID
const char* password = "3sL44wR6";    //パスワード
const int port = 5000;                //ポート番号

int mode[4][3]={{255,228,206},{255,165,0},{255,0,0},{0,0,0}};//それぞれのモードのRGBを二次元配列で管理　 [0: 集中   1: リラックス  2: 警告   3:照明なし]
int past_num,rec_data,past_rec_data; //wifiからのモード番号受信，現在のモードと直前のモードを格納

/////////////////////三角波で輝度＋色を段階的に変化させる関数///////////////////////////////////

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
    delay(20); //even shorter delay this time}

  }

}

////////////////////////////////////////////////////////////////////////////////////////

void setup() {

M5.begin(true, false, true);  //Serial,I2C,LED

FastLED.addLeds<CHIPSET,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MAX_AMPS);
FastLED.setBrightness(BRIGHTNESS);
FastLED.clear();
FastLED.show(); 

/////////////////////////////////Wifiの通信接続/////////////////////////////////////////////

WiFi.config(IPAddress(192, 168, 0, 100), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 0, 1));
    WiFi.begin(ssid, password);

    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {

      Serial.println("WiFi not connected, retrying...");
      delay(1000);

    }

    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.print(WiFi.localIP());
    udp.begin(port);
    Serial.println("UDP server started.");


  //接続判定用LED
    for(int i=0; i<25; i++) {
    M5.dis.drawpix(i, CRGB::Blue);
    delay(10);
  }

//////////////////////////////////////////////////////////////////////////////////////////
}

void loop() { 

M5.update();
past_rec_data=rec_data;//モードの更新

////////////////////////////データの受信/////////////////////////////////////////////

char packetBuffer[255];
    int packetSize = udp.parsePacket();
    
    if (packetSize) {
        udp.read(packetBuffer, packetSize);
        packetBuffer[packetSize] = 0;
        Serial.println("Received: " + String(packetBuffer));
        rec_data=atoi(packetBuffer);
    }


    //モード変更時，色＋輝度を変化させる関数の呼び出し
    if(past_rec_data!=rec_data){
      LED_bright_dark(past_rec_data, rec_data);
    }

///////////////////////////////LEDの制御//////////////////////////////////

  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(mode[rec_data][0],mode[rec_data][1] ,mode[rec_data][2]);
    FastLED.show();
    delay(10); 
  }
}