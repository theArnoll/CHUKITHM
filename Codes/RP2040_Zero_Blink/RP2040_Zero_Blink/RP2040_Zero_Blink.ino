#include <Adafruit_NeoPixel.h>

// RP2040-Zero 的內建 RGB LED 連接在 GPIO 16
#define RGB_PIN 16
#define NUMPIXELS 1

// 初始化 NeoPixel 物件
Adafruit_NeoPixel pixels(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(2000000);
    while (!Serial) {
        ;  // Wait for Serial be ready, or else it won't work
    }
    pixels.begin();            // 啟動 WS2812 LED
    pixels.setBrightness(50);  // 設定亮度 (0 到 255)。建議設定在 50 就很亮了，太高會刺眼！
}

void loop() {
  // 三層迴圈：這次我們加了 delay，所以 +=25 讓它跳快一點，不然會變太慢
    for (int a = 0; a < 256; a += 5) {
        for (int b = 0; b < 256; b += 5) {
            for (int c = 0; c < 256; c += 5) {
                pixels.setPixelColor(0, pixels.Color(a, b, c));
                pixels.show();
                Serial.print(a); Serial.print(" ");
                Serial.print(b); Serial.print(" ");
                Serial.println(c);
            }
        }
    }
    
    // 讓 LED 熄滅
    pixels.clear(); 
    pixels.show();
    delay(1000);    // 這樣這裡的暗一秒，就會跟前面的「慢速漸變」有明顯的區隔了！
}