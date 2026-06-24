// --- ESP32-C3 腳位定義 ---
#include <Arduino.h>

// IR 與 TSSP 感測器
const int irLedPin = 5;  
const int tsspPin1 = 6;  
const int tsspPin2 = 7;  

// 單顆 74HC595
const int dataPin = 0;   // DS
const int clockPin = 1;  // SHCP
const int latchPin = 3;  // STCP

void setup() {
    Serial.begin(115200);
    
    // 設定 TSSP 為輸入
    pinMode(tsspPin1, INPUT_PULLUP);
    pinMode(tsspPin2, INPUT_PULLUP);

    // 設定 595 控制腳位
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    ledcAttach(irLedPin, 38000, 8); 
    // 【關鍵調整】將佔空比從 128 (50%) 降到 20 (約 8%)
    // 如果覺得感測距離太近，可以慢慢調高這個數字 (例如 40, 60)
    ledcWrite(irLedPin, 32);  
}

// 寫入 8 bits 資料到單顆 595
void write595(uint8_t data) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, data);
    digitalWrite(latchPin, HIGH);
}

void loop() {
  // 讀取感測器狀態 (LOW 代表被觸發/接收到反射)
    int val1 = digitalRead(tsspPin1);
    int val2 = digitalRead(tsspPin2);

    uint8_t ledPattern = 0; // 準備送給 595 的 8-bit 資料

    // 邏輯：手靠近 (LOW) -> 點亮對應的 LED (設定該 bit 為 1)
    if (val1 == LOW) {
        ledPattern |= (1 << 0); // 點亮 595 的 Q0 (第 1 顆 LED)
    }
    
    if (val2 == LOW) {
        ledPattern |= (1 << 1); // 點亮 595 的 Q1 (第 2 顆 LED)
    }

    // 將結果送到 74HC595
    write595(ledPattern);

    // 稍微延遲，避免不必要的頻繁刷新
    delay(10); 
}