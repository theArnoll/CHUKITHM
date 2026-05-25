#pragma once
#include "Adafruit_TinyUSB.h"

// 112 鍵 NKRO 描述符
uint8_t const desc_hid_report[] = {
  0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x85, 0x01, 
  0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00, 
  0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 
  0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x19, 0x00, 
  0x29, 0x6F, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 
  0x95, 0x70, 0x81, 0x02, 0xC0              
};

class NKROKeyboard {
private:
  Adafruit_USBD_HID usb_hid;
  uint8_t report[16];

  uint8_t charToHID(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a' + 0x04;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 0x04;
    if (c >= '1' && c <= '9') return c - '1' + 0x1E;
    if (c == '0') return 0x27;
    if (c == ' ') return 0x2C;
    return 0; 
  }

public:
  NKROKeyboard() { memset(report, 0, 16); }

  void begin() {
    // 1. 如果 TinyUSB 還沒初始化，強制初始化
    if (!TinyUSBDevice.isInitialized()) {
      TinyUSBDevice.begin(0);
    }

    // 2. 設定 1000Hz 輪詢率與描述符
    usb_hid.setPollInterval(1); 
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();

    // 3. 【核心救星：強迫重新枚舉】
    // 如果已經連線，強制軟體「拔掉 100ms 再插回」，逼電腦重刷裝置清單
    if (TinyUSBDevice.mounted()) {
      TinyUSBDevice.detach();
      delay(100); 
      TinyUSBDevice.attach();
    }
  }

  void add(char c) {
    uint8_t keycode = charToHID(c);
    if (keycode == 0 || keycode > 111) return;
    report[2 + (keycode / 8)] |= (1 << (keycode % 8));
  }

  void release(char c) {
    uint8_t keycode = charToHID(c);
    if (keycode == 0 || keycode > 111) return;
    report[2 + (keycode / 8)] &= ~(1 << (keycode % 8));
  }

  void releaseAll() {
    memset(report, 0, 16);
  }

  void send() {
    if (TinyUSBDevice.suspended()) {
      TinyUSBDevice.remoteWakeup();
    }
    
    if (!usb_hid.ready()) return; // 確保端點準備好再發送，不卡死 loop
    usb_hid.sendReport(1, report, sizeof(report));
  }
};

extern NKROKeyboard KB;
NKROKeyboard KB;