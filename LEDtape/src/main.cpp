#include <M5Atom.h>
#include <FastLED.h>

#define NUM_LEDS 55      //LEDの個数
#define DATA_PIN 22      //LED制御に使用するGPIOピン
#define COLOR_ORDER GRB  //Green (G), Red (R), Blue (B)
#define CHIPSET WS2812B
#define BRIGHTNESS 255  //LEDの輝度
#define VOLTS 5
#define MAX_AMPS 500  //value in milliamps
#define BUFSIZE 1024
#define DELAYVAL 100  //LEDのピクセル間でのdelay

CRGB leds[NUM_LEDS];
String recieve;

//それぞれのモードのRGBを二次元配列で管理//[0:照明なし, 1: 集中   2: リラックス  3: 警告 ]   旧Ver [3:照明なし, 0: 集中   1: リラックス  2: 警告 ] 
int mode[4][3] = { { 0, 0, 0 } , { 255, 228, 206 }, { 255, 135, 25 }, { 255, 0, 0 }};  
int ledcheck[3] = { 242, 72, 34 };                                                    //接続判定用LEDのRGB
int past_num, rec_data, past_rec_data;                                                //bluetoothからのモード番号受信，現在のモードと直前のモードを格納
bool connect_check = false;
CRGB dispColor(uint8_t r, uint8_t g, uint8_t b) {return (CRGB)((r << 16) | (g << 8) | b);}

/////////////////////三角波で輝度＋色を段階的に変化させる関数///////////////////////////////////
void LED_bright_dark(int past, int now) {
  if (now == 2) {  // 警告時，瞬時に赤色に点灯
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(mode[3][0], mode[3][1], mode[3][2]);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
    delay(20);
  } 
  else if (past==2 && now ==0){ //警告モードからの復帰時は瞬時に白色に点灯
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(mode[1][0], mode[1][1], mode[1][2]);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
    delay(20);
  }

  else {  //通常，モード切り替え時は緩やかに LED が明滅
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(mode[past][0], mode[past][1], mode[past][2]);
    for (int i = BRIGHTNESS; i > 0; i -= 3) {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(20);
    }
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(mode[now][0], mode[now][1], mode[now][2]);
    for (int i = 0; i < BRIGHTNESS; i += 3) {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(20);  //even shorter delay this time}
    }
  }
}

////////////////////////////////Bluetooth LE////////////////////////////////////////////////////////
#include <NimBLEDevice.h>

//デバイス名
#define SERVER_NAME                   "M5Stack"
#define COMPLETE_SERVER_NAME          "M5Stack"

//固有UUID
#define SERVICE_UUID                  "FE55"
#define CHARACTERISTIC_UUID           "739e3a92-83ad-11ee-b962-0242ac120002"
#define CHARACTERISTIC_UUID_NOTIFY    "739e3e34-83ad-11ee-b962-0242ac120002"

//Characteristic
NimBLECharacteristic* pCharacteristic = NULL;
//NimBLEServer
NimBLEServer* pServer = NULL;

//接続状態管理
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool isConnected = false;

//Serviceのコールバック
class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer) {
    BLEDevice::stopAdvertising();
    Serial.println("onConnect");
    deviceConnected = true;
  };
  void onDisconnect(NimBLEServer* pServer) {
    BLEDevice::startAdvertising();
    Serial.println("onDisconnect");
    deviceConnected = false;
  }
  void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc) {
      Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
  };
};

//BLECharacteristicコールバック
class MyCallbacks : public BLECharacteristicCallbacks {
  //Writeコールバック
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      recieve = String(rxValue.c_str());
      Serial.print("Received Value : ");
      Serial.println(recieve);
    }
  }
};

// BLE初期化
void initBLE() {
  Serial.print("BLE setup start!");
  //CompleteLocalNameのセット
  NimBLEDevice::init(COMPLETE_SERVER_NAME);
  //TxPowerのセット
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  // セキュリティ無し
  NimBLEDevice::setSecurityAuth(false, false, false);
  //サーバー作成
  pServer = BLEDevice::createServer();
  //サーバーにコールバック設定
  pServer->setCallbacks(new MyServerCallbacks());
  //サーバーにUUIDをセット
  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE |
    NIMBLE_PROPERTY::WRITE_NR |
    NIMBLE_PROPERTY::NOTIFY |
    NIMBLE_PROPERTY::INDICATE
  );
  pCharacteristic->setCallbacks(new MyCallbacks());

  //Service開始
  pService->start();
  //アドバタイズの設定
  NimBLEAdvertising *pNimBleAdvertising = NimBLEDevice::getAdvertising();
  //アドバタイズするUUIDのセット
  pNimBleAdvertising->addServiceUUID(SERVICE_UUID);
  //アドバタイズにTxPowerセット
  pNimBleAdvertising->addTxPower();

  // アドバタイズデータ作成
  NimBLEAdvertisementData advertisementData;
  // アドバタイズにCompleteLocalNameセット
  advertisementData.setName(COMPLETE_SERVER_NAME);  
  // アドバタイズのManufactureSpecificにデータセット
  advertisementData.setManufacturerData("NORA");  
  // ScanResponseを行う
  pNimBleAdvertising->setScanResponse(true);
  // ScanResponseにアドバタイズデータセット
  pNimBleAdvertising->setScanResponseData(advertisementData);  
  //アドバタイズ開始
  pNimBleAdvertising->start();

  Serial.println("→→→setup complete!");
}

//BLE Loop
void loopBLE() {
  // 切断中
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                   // BLEスタックの準備時間
    pServer->startAdvertising();  // アドバタイズの再開
    Serial.println("startAdvertising");
    oldDeviceConnected = deviceConnected;
    connect_check=false;

    for (int i = 0; i < 25; i++) {
      M5.dis.drawpix(i,0);
      FastLED.show();
      delay(10);
    }
    recieve ='3';  //通信切断時，LEDライトを消灯
  }

  // 接続中
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
    isConnected = true;
    connect_check=true;
    //接続判定用LED
    for (int i = 0; i < 25; i++) {
      M5.dis.drawpix(i,dispColor(ledcheck[0],ledcheck[1],ledcheck[2]));
      delay(10);
    }
  }
}

////////////////////////////////SetUp & Loop////////////////////////////////////////////////////////
void setup() {
  M5.begin(true, false, true);  //Serial,I2C,LED
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_AMPS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  initBLE();
}

void loop() {
  M5.update();
  past_rec_data = rec_data;  //モードの更新
  loopBLE();

  if(connect_check)rec_data = recieve.toInt();//接続している時だけデータの読み取り
  else rec_data=3; //接続してない時は消灯
  
//モード変更時，色＋輝度を変化させる関数の呼び出し
  if (past_rec_data != rec_data) {
    LED_bright_dark(past_rec_data, rec_data);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(mode[rec_data][0], mode[rec_data][1], mode[rec_data][2]);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
    delay(10);
  }
  Serial.println(rec_data);
}