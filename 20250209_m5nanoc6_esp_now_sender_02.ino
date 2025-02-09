


//ESP32_NOW_m_button1_M5NanoC6


//インクルド
#include <M5NanoC6.h> //SW
#include "ESP32_NOW.h"
#include "WiFi.h"
#include <esp_mac.h>  // For the MAC2STR and MACSTR macros


//定義
/* Definitions */

//WiFiチャンネルの指定
#define ESPNOW_WIFI_CHANNEL 1

//クラス
/* Classes */

// Creating a new class that inherits from the ESP_NOW_Peer class is required.

class ESP_NOW_Broadcast_Peer : public ESP_NOW_Peer {
  public:
    // Constructor of the class using the broadcast address
    ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) : ESP_NOW_Peer(ESP_NOW.BROADCAST_ADDR, channel, iface, lmk) {}

    // Destructor of the class
    ~ESP_NOW_Broadcast_Peer() {
      remove();
    }

    // Function to properly initialize the ESP-NOW and register the broadcast peer
    bool begin() {
      if (!ESP_NOW.begin() || !add()) {
        log_e("Failed to initialize ESP-NOW or register the broadcast peer");
        return false;
      }
      return true;
    }

    // Function to send a message to all devices within the network
    bool send_message(const uint8_t *data, size_t len) {
      if (!send(data, len)) {
        log_e("Failed to broadcast message");
        return false;
      }
      return true;
    }
};

/* Global Variables */

uint32_t msg_count = 0;

//ブロードキャスト オブジェクトの定義
// Create a broadcast peer object
ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);

/* Main */


//初期化
void setup() {

  NanoC6.begin(); //SW

  //WiFiの初期化
  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }//while

  //ブロードキャストの初期化
  // Register the broadcast peer
  if (!broadcast_peer.begin()) {
    delay(1000);
    ESP.restart();
  }//endif

  //スタートメッセージを送る debug
  //delay(5000);
  //                                           123456789012345
  //if (!broadcast_peer.send_message((uint8_t *)"Start SYSTEM!!\n", 15)) {
    //ERR
  //}//endif

}//setup


//メインループ
void loop() {
  // Broadcast a message to all devices within the network
  static char data[32];

  //ボタンの状態の更新
  NanoC6.update();

  //ボタンを押した時
  if (NanoC6.BtnA.wasPressed()) {

    snprintf(data, sizeof(data), "Button A was pressed #%lu", msg_count++);

    if (!broadcast_peer.send_message((uint8_t *)data, sizeof(data))) {
      //ERR
    }//endif

  }//endif

  //ボタンを押し続けた時
  if (NanoC6.BtnA.wasHold()) {

    snprintf(data, sizeof(data), "Button A was hold #%lu", msg_count++);

    if (!broadcast_peer.send_message((uint8_t *)data, sizeof(data))) {
      //ERR
    }//endif

  }//endif

  //ボタンを離した時
  if (NanoC6.BtnA.wasReleased()) {

    snprintf(data, sizeof(data), "Button A was released #%lu", msg_count++);

    if (!broadcast_peer.send_message((uint8_t *)data, sizeof(data))) {
      //ERR
    }//endif

  }//endif

  delay(1);//ダミー

}//loop


