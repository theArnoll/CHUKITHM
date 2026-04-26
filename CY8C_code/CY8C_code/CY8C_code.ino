#include <CypressCY8CMBR3116.h>
#include <Wire.h>

#define I2C_ADDRESS 0x51      // 硬體配置的 I2C 地址，預設是 0x51
#define REQUEST_TIMEOUT 40    // 定義超時次數

// 創建 CY8CMBR3116 指令實例
CY8CMBR3116 touchIC(I2C_ADDRESS, REQUEST_TIMEOUT);

void setup() {
    Serial.begin(9600);
    Wire.begin();
    Serial.println("Start Program");
}

void loop() {
    requestTouchStatus();
    delay(500); // 延遲 0.5 秒以減少數據頻率
}

// 要求觸控狀態的函數
void requestTouchStatus() {
    Serial.println("Requesting touch status...");

    uint8_t touchStatusBuffer[2];
    uint8_t error = touchIC.get_BUTTON_STAT(touchStatusBuffer);

    if (error != 0) {
        printError(error);
    } else {
        printStatus(touchStatusBuffer);
    }
}

// 打印觸控狀態
void printStatus(uint8_t *statusBuffer) {
    Serial.println("Touch Status: ");

    // 假設有16個按鈕，每位代表一個按鈕的狀態（0未觸發，1已觸發）
    for (int i = 0; i < 8; i++) {
        uint8_t pressed = statusBuffer[0] >> i & 0b00000001;
        Serial.print(pressed);
        Serial.print(", ");
    }
    for (int i = 0; i < 7; i++) {
        uint8_t pressed = statusBuffer[1] >> i & 0b00000001;
        Serial.print(pressed);
        Serial.print(", ");
    }

    // 最後一位狀態
    uint8_t pressed = statusBuffer[1] >> 7 & 0b00000001;
    Serial.println();
}

// 打印錯誤訊息
void printError(uint8_t errorCode) {
    Serial.println("Communication Error:");
    switch (errorCode) {
        case 1:
            Serial.println("Error 1: Data too long to fit in transmit buffer");
            break;
        case 2:
            Serial.println("Error 2: Received NACK on transmit of address");
            break;
        case 3:
            Serial.println("Error 3: Received NACK on transmit of data");
            break;
        case 4:
            Serial.println("Error 4: Other Error");
            break;
        case 5:
            Serial.println("Error 5: Timeout Error");
            break;
        default:
            Serial.println("Unknown Wire Error");
            break;
    }
}