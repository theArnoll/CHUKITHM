#include <CypressCY8CMBR3116.h>
#include <Wire.h>
#include "NKRO_TinyUSB.h" // 【修改 1】換成我們的 TinyUSB NKRO 標頭檔

#define I2C_ADDRESS 0x51     //I2C Address of the Cypress IC (0x37 is default)
#define REQUEST_TIMEOUT 40   //After how many request is a Timeout triggered

CY8CMBR3116 touchIC(I2C_ADDRESS, REQUEST_TIMEOUT);

void setup() {
    Serial.begin(115200);
    
    Wire.setSDA(0);
    Wire.setSCL(1);
    Wire.begin();   
    Wire.setClock(400000);  // 400kHz I²C fast mode
    
    // delay(2);
    while (!Serial) { ; } 

    Wire.beginTransmission(I2C_ADDRESS);
    if (Wire.endTransmission() == 0) Serial.println("Device found");
    delay(50);  // 50ms prepare wake time. To wake up the IC.

    pinMode(10, OUTPUT);
    digitalWrite(10, 0);
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);

    if(!digitalRead(8)) KB.begin(); // 【修改 3】
    Serial.println("Start Program");
}

//   -------------------------------------------

// char keymap[6] = { 'e', 'r', 't', 'u', 'i', 'o' };
char keymap[2][8] = { { '0', '0', '0', '0', 'r', 'r', 'e', 'e' } ,
                      { 'o', 'o', 'i', 'i', 'u', 'u', 't', 't' } };
                    //   0    1    2    3    4    5    6    7
uint8_t lastStatus[2] = { 0, 0 };

void loop() {
    uint8_t touchStatus[2];
    requestTouchStatus(touchStatus);

    if(digitalRead(7) == LOW)
    {
        Serial.print("|");
        for (int i = 0; i < 8; i++)
        {
            uint8_t pressed = touchStatus[0] >> i & 0b00000001;
            Serial.print((char)(pressed ? (i+'0') : '-'));
            Serial.print("|");
        }   Serial.print(" |");
        for (int i = 0; i < 8; i++)
        {
            uint8_t pressed = touchStatus[1] >> i & 0b00000001;
            Serial.print((char)(pressed ? (i+'0') : '-'));
            Serial.print("|");
        }   Serial.println();
    }
    
    if (    digitalRead(8) == LOW &&
        ( !(lastStatus[0] == 0x55 && lastStatus[1] == 0x55 
        && touchStatus[0] == 0xAA && touchStatus[1] == 0xAA))
        && (lastStatus[0] != touchStatus[0] || lastStatus[1] != touchStatus[1]) )
    {   for (uint8_t bfrsel = 0; bfrsel <= 1; bfrsel++)
        {   for (int i = 0; i < 8; i++)
            {   uint8_t pressed = touchStatus[bfrsel] >> i & 0b00000001, lastPressed = lastStatus[bfrsel] >> i & 0b00000001;
                if(pressed && (pressed != lastPressed))
                {
                    KB.add(keymap[bfrsel][i]);
                    if(!digitalRead(7)) Serial.println(i);
                }
                else {
                    if(pressed != lastPressed)
                    {
                        KB.release(keymap[bfrsel][i]);
                        if(!digitalRead(7)) { Serial.print("-"); Serial.println(i); }
                    }
                }
            }
        }
        
    }

    KB.send();
    lastStatus[0] = touchStatus[0]; lastStatus[1] = touchStatus[1];
}

void requestTouchStatus(uint8_t statusStorage[2])
{
    uint8_t touchStatusBuffer[2];
    uint8_t error = touchIC.get_BUTTON_STAT(touchStatusBuffer);

    if (error != 0)
    {
        Serial.print("Request Touch Status Loop: ");
        printError(error);
        for (uint8_t re = 0; re < 2; re++)
            touchStatusBuffer[re] = (touchStatusBuffer[re] == 0x55 ? 0xAA : 0x55);
    }

    statusStorage[0] = touchStatusBuffer[0];
    statusStorage[1] = touchStatusBuffer[1];
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