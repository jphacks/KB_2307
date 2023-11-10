#include <FastLED.h>
#include<M5Atom.h>
#include<BluetoothSerial.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define NUM_LEDS 60 //LEDの個数
#define DATA_PIN 22 //LED制御に使用するGPIOピン
#define COLOR_ORDER GRB //Green (G), Red (R), Blue (B)
#define CHIPSET WS2812B
#define BRIGHTNESS 255 //LEDの輝度
#define VOLTS 5
#define MAX_AMPS 500 //value in milliamps
#define BUFSIZE 1024
#define DELAYVAL 100 //LEDのピクセル間でのdelay

CRGB leds[NUM_LEDS];

String recieve;


//255,228,206
int mode[4][3]={{255,228,206},{255,165,0},{255,0,0},{0,0,0}};//それぞれのモードのRGBを二次元配列で管理　 [0: 集中   1: リラックス  2: 警告   3:照明なし]
int past_num,rec_data,past_rec_data; //bluetoothからのモード番号受信，現在のモードと直前のモードを格納


/////////////////////Bluetooth//////////////////////////////////////////////////////
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

bool connected = false;

/////////////////////三角波で輝度＋色を段階的に変化させる関数///////////////////////////////////

void LED_bright_dark(int past, int now){

  for(int i=0;i<NUM_LEDS;i++)leds[i] = CRGB(mode[past][0],mode[past][1] ,mode[past][2]);
  for(int i=BRIGHTNESS;i>0;i-=3){ 
    FastLED.setBrightness(i);
    FastLED.show();
    delay(20);

  }


  for(int i=0;i<NUM_LEDS;i++)leds[i] = CRGB(mode[now][0],mode[now][1] ,mode[now][2]);
  for(int i=0;i<BRIGHTNESS;i+=3){ 
    FastLED.setBrightness(i);
    FastLED.show();
    delay(20); //even shorter delay this time}

  }

}

////////////////////////////////////////////////////////////////////////////////////////

///////////////////
// Bluetooth LE  //
///////////////////
BLEServer *pServer = NULL;
BLECharacteristic * pNotifyCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define LOCAL_NAME                  "M5Stack-Matrix"
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID                "e5a1c9a8-ab93-11e8-98d0-529269fb1459"
#define CHARACTERISTIC_UUID_RX      "e5a1cda4-ab93-11e8-98d0-529269fb1459"
#define CHARACTERISTIC_UUID_NOTIFY  "e5a1d146-ab93-11e8-98d0-529269fb1459"

// Bluetooth LE Change Connect State
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

// Bluetooth LE Recive
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        recieve = String(rxValue.c_str());
        Serial.print("Received Value: ");
        Serial.println(recieve);
      }
    }
};

// Bluetooth LE initialize
void initBLE() {
  // Create the BLE Device
  BLEDevice::init(LOCAL_NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pNotifyCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_NOTIFY,
                        BLECharacteristic::PROPERTY_NOTIFY
                        );
  
  pNotifyCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  Serial.println("connecting!!");
}


// Bluetooth LE loop
void loopBLE() {
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("startAdvertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
          //接続判定用LED
        for(int i=0; i<25; i++) {
        M5.dis.drawpix(i, CRGB::Orange);
        delay(10);
  }
    }
}

void setup() {

M5.begin(true, false, true);  //Serial,I2C,LED

FastLED.addLeds<CHIPSET,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MAX_AMPS);
FastLED.setBrightness(BRIGHTNESS);
FastLED.clear();
FastLED.show(); 

initBLE();

}

void loop() { 

M5.update();
past_rec_data=rec_data;//モードの更新
loopBLE();

rec_data=recieve.toInt();


    //モード変更時，色＋輝度を変化させる関数の呼び出し
    if(past_rec_data!=rec_data){
      LED_bright_dark(past_rec_data, rec_data);
    }


  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(mode[rec_data][0],mode[rec_data][1] ,mode[rec_data][2]);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
    delay(10); 
  }


  Serial.println(rec_data);
}


