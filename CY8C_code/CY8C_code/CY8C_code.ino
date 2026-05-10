#include <CypressCY8CMBR3116.h>
#include <Wire.h>
#include <HID-Project.h>

#define I2C_ADDRESS 0x51     //I2C Address of the Cypress IC (0x37 is default)
#define REQUEST_TIMEOUT 40   //After how many request is a Timeout triggered

CY8CMBR3116 touchIC(I2C_ADDRESS, REQUEST_TIMEOUT);

void setup() {
    Serial.begin(115200);
    Wire.begin();   // SDA -> 2, SCL -> 3
    Wire.setClock(400000);  // 400kHz I²C fast mode
    while (!Serial) {
        ;  // Wait for Serial be ready, or else it won't work
    }

    Wire.beginTransmission(I2C_ADDRESS);
    if (Wire.endTransmission() == 0) Serial.println("Device found");
    delay(50);  // 50ms prepare wake time. To wake up the IC.
    
    uint8_t val_CFG2 = 0b00001000; // Disable button auto-reset (default 1 is going to make the button report as untouched even if the finger is still touching after 5 seconds)
    uint8_t tmp = touchIC.set_DEVICE_CFG2(&val_CFG2);
    if(tmp){ Serial.print("set_DEVICE_CFG2: "); printError(tmp); digitalWrite(10, 1); }
    uint8_t buf[1];
    Serial.print("DEVICE_CFG2: ");
    touchIC.get_DEVICE_CFG2(buf);
    Serial.println(buf[0]);

    uint8_t val_REFRESH = 0x01; // 20ms scan period / refresh period
    tmp = touchIC.set_REFRESH_CTRL(&val_REFRESH);
    if(tmp){ Serial.print("set_REFRESH_CTRL: "); printError(tmp); digitalWrite(10, 1); }
    Serial.print("REFRESH_CTRL: ");
    touchIC.get_REFRESH_CTRL(buf);
    Serial.println(buf[0]);

    uint8_t val_DEBOUNCE = 0x01; // Minimize debounce (the defaut 3 is likely to triple the 20ms refresh period, which is going to increase the latency.)
    tmp = touchIC.set_SENSOR_DEBOUNCE(&val_DEBOUNCE);
    if(tmp){ Serial.print("set_SENSOR_DEBOUNCE: "); printError(tmp); digitalWrite(10, 1); }
    Serial.print("SENSOR_DEBOUNCE: ");
    touchIC.get_SENSOR_DEBOUNCE(buf);
    Serial.println(buf[0]);

    uint8_t val_TIMEOUT = 0x3F; // Keep wake for 63 second
    tmp = touchIC.set_STATE_TIMEOUT(&val_TIMEOUT);
    if(tmp){ Serial.print("set_STATE_TIMEOUT: "); printError(tmp); digitalWrite(10, 1); }
    Serial.print("STATE_TIMEOUT: ");
    touchIC.get_STATE_TIMEOUT(buf);
    Serial.println(buf[0]);

    tmp = touchIC.activateSettings(); // I may not need to comment this, right?
    if(tmp){ Serial.print("Activate Settings: "); printError(tmp); digitalWrite(10, 1); }

    delay(100);

    pinMode(10, OUTPUT);
    digitalWrite(10, 0);
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);

    NKROKeyboard.begin();
    Serial.println("Start Program");
}

//   -------------------------------------------

char keymap[6] = { 'e', 'r', 't', 'u', 'i', 'o' };
// char keymap[8] = { 'w', 'e', 'r', 't', 'u', 'i', 'o', 'p' };
uint8_t lastStatus[2] = { 0, 0 };

void loop() {
    uint8_t touchStatus[2];
    requestTouchStatus(touchStatus);

    if(digitalRead(7) == LOW)
    {
        Serial.print("| ");
        for (int i = 0; i < 6; i++)
        {
            uint8_t pressed = touchStatus[0] >> i & 0b00000001;
            Serial.print(pressed);
            Serial.print(" | ");
        }   Serial.println();
    }
    
    // if (toggleStarted && status != warning && touchStatus.changed()) send key press accordingly;
    if (    digitalRead(8) == LOW &&
        ( !(lastStatus[0] == 0x55 && lastStatus[1] == 0x55 
        && touchStatus[0] == 0xAA && touchStatus[1] == 0xAA))
        && (lastStatus[0] != touchStatus[0] || lastStatus[1] != touchStatus[1]) )
    {   for (int i = 0; i < 6; i++)
        {   uint8_t pressed = touchStatus[0] >> i & 0b00000001, lastPressed = lastStatus[0] >> i & 0b00000001;
            if(pressed && (pressed != lastPressed))
            {
                NKROKeyboard.add(keymap[i]);
                if(!digitalRead(7)) Serial.println(i);
            }
            else {
                if(pressed != lastPressed)
                {
                    NKROKeyboard.release(keymap[i]);
                    if(!digitalRead(7)) { Serial.print("-"); Serial.println(i); }
                }
            }
        }
    }

    NKROKeyboard.send();
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

    // Status of the last bit
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