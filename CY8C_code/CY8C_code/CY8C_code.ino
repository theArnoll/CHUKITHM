#include <CypressCY8CMBR3116.h>
#include <Wire.h>
#include <HID-Project.h>

#define I2C_ADDRESS 0x51     //I2C Address of the Cypress IC (0x37 is default)
#define REQUEST_TIMEOUT 40   //After how many request is a Timeout triggered

// Create an instance of the IC
CY8CMBR3116 touchIC(I2C_ADDRESS, REQUEST_TIMEOUT);

void setup() {
    Serial.begin(9600);
    Wire.begin();  // SDA -> 2, SCL -> 3
    Wire.beginTransmission(I2C_ADDRESS); // 呼叫 CY8CMBR3116 的預設位址
    if (Wire.endTransmission() == 0) Serial.print("Device found");
    delay(50);                    // 給予 50 毫秒的絕對清醒時間，讓它準備好

    pinMode(6, INPUT_PULLUP);

    NKROKeyboard.begin();
    Serial.println("Start Program");
}

//   -------------------------------------------

// char keymap[8] = {  ',', '.' , 'p', 'y', 'g', 'c', 'r', 'l' };
char keymap[6] = { ',', 'p', 'y', 'g', 'c', 'r' };
// char keymap[8] = { 'w', 'e', 'r', 't', 'u', 'i', 'o', 'p' };
uint8_t lastStatus[2] = { 0, 0 };

void loop() {
    uint8_t touchStatus[2];
    requestTouchStatus(touchStatus);
    for (int i = 0; i < 6; i++)
    {
        uint8_t pressed = touchStatus[0] >> i & 0b00000001;
        Serial.print(pressed);
        Serial.print(". ");
    }
    if (    digitalRead(6) == LOW &&
        ( !(lastStatus[0] == 0x55 && lastStatus[1] == 0x55 
        && touchStatus[0] == 0xAA && touchStatus[1] == 0xAA)))
    {
        for (int i = 0; i < 6; i++)
        {   Serial.print("|");
            uint8_t pressed = touchStatus[0] >> i & 0b00000001;
            if(pressed)
            {
                NKROKeyboard.add(keymap[i]);
                Serial.print("X");
            }
            else
            {
                NKROKeyboard.release(keymap[i]);
                Serial.print("O");
            }
        }
        Serial.print("|");
    }
    lastStatus[0] = touchStatus[0]; lastStatus[1] = touchStatus[1];
    delay(1);
}

void requestTouchStatus(uint8_t statusStorage[2])
{
    uint8_t touchStatusBuffer[2];
    uint8_t error = touchIC.get_BUTTON_STAT(touchStatusBuffer);

    if (error != 0)
    {
        printError(error);
        for (uint8_t re = 0; re < 2; re++)
            touchStatusBuffer[re] = (touchStatusBuffer[re] == 0x55 ? 0xAA : 0x55);
    }
    else
    {
        Serial.print(" ");
        printStatus(touchStatusBuffer);
    }

    statusStorage[0] = touchStatusBuffer[0];
    statusStorage[1] = touchStatusBuffer[1];
}

void printStatus(uint8_t *statusBuffer)
{
    // Original         8
    for (int i = 0; i < 6; i++)
    {
        uint8_t pressed = statusBuffer[0] >> i & 0b00000001;
        Serial.print(pressed);
        Serial.print(", ");
    }
    /* for (int i = 0; i < 7; i++)
    {
        uint8_t pressed = statusBuffer[1] >> i & 0b00000001;
        Serial.print(pressed);
        Serial.print(", ");
    }

    最後一位狀態
    uint8_t pressed = statusBuffer[1] >> 7 & 0b00000001; */
    Serial.println();
}

// 打印錯誤訊息
void printError(uint8_t errorCode)
{
    Serial.print("Communication Error ");
    switch (errorCode)
    {
        case 1:
            Serial.println("1: Data too long to fit in transmit buffer");
            break;
        case 2:
            Serial.println("2: Received NACK on transmit of address");
            break;
        case 3:
            Serial.println("3: Received NACK on transmit of data");
            break;
        case 4:
            Serial.println("4: Other Error");
            break;
        case 5:
            Serial.println("5: Timeout Error");
            break;
        default:
            Serial.print("Unknown: Unknown Wire Error ");
            Serial.println(errorCode);
            break;
    }
}