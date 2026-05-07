#define IR_TX_PIN 4  // 紅外線發射 (記得串聯 220 歐姆左右的限流電阻)
#define IR_RX_PIN 5  // 38kHz 接收 IC 的 OUT 腳

void setup() {
  pinMode(IR_RX_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  // 1. 發射端：打出一陣極短的 38kHz 脈衝 (Burst)
  tone(IR_TX_PIN, 38000); 
  
  // 給接收 IC 一點點時間反應 (大約 500 到 800 微秒即可)
  delayMicroseconds(500); 

  // 2. 接收端：讀取當下狀態 (收到 38kHz 時是 LOW，沒收到是 HIGH)
  int rx_state = digitalRead(IR_RX_PIN);

  // 3. 立即關閉發射端，讓接收 IC 的 AGC 休息，避免它視覺疲勞
  noTone(IR_TX_PIN);

  // 4. 邏輯判斷 (假設你是做遮斷式 AIR 鍵，手揮過去會擋住光)
  if (rx_state == HIGH) {
    // 光被擋住了，代表手在上面！
    Serial.println(1);
    // 這裡可以換成 NKROKeyboard.add(' ');
  } else Serial.println(0);

  // 5. 休息一段極短的時間，再進行下一次掃描 (控制整體延遲在 2~3ms 內)
  // delay(2); 
}