#include <CypressCY8CMBR3116.h>
#include <Wire.h>
#include "NKRO_TinyUSB.h" // TinyUSB NKRO
#include <Adafruit_NeoPixel.h>
#include <SPI.h>

#define I2C_ADDRESS 0x51     //I2C Address of the Cypress IC (0x37 is default)
#define REQUEST_TIMEOUT 40   //After how many request is a Timeout triggered

CY8CMBR3116 touchIC(I2C_ADDRESS, REQUEST_TIMEOUT);
Adafruit_NeoPixel pixels(1, 16, NEO_GRB + NEO_KHZ800);

void printError(uint8_t errorCode)
{
    Serial.print("Communication Error ");
    switch (errorCode)
    {
        case 1: Serial.println("1: Data too long to fit in transmit buffer");
                pixels.setPixelColor(0, pixels.Color(255, 0, 0)); pixels.show(); break; // red
        case 2: Serial.println("2: Received NACK on transmit of address"); 
                pixels.setPixelColor(0, pixels.Color(0, 255, 0)); pixels.show(); break; // green
        case 3: Serial.println("3: Received NACK on transmit of data"); 
                pixels.setPixelColor(0, pixels.Color(0, 0, 255)); pixels.show(); break;  // blue
        case 4: Serial.println("4: Other Error");
                pixels.setPixelColor(0, pixels.Color(255, 255, 0)); pixels.show(); break; // yellow
        case 5: Serial.println("5: Timeout Error"); 
                pixels.setPixelColor(0, pixels.Color(255, 0, 255)); pixels.show(); break; // purple
        default:
            Serial.print("Unknown: Unknown Wire Error ");
            Serial.println(errorCode);
            pixels.setPixelColor(0, pixels.Color(255, 255, 255)); pixels.show();
            break;
    }
}

void requestTouchStatus(uint8_t statusStorage[2][2])
{
    uint8_t touchStatusBuffer[2];
    uint8_t error = touchIC.get_BUTTON_STAT(touchStatusBuffer);
    if (error != 0) { Serial.print("Request Touch Status Loop: "); printError(error); }
                else { pixels.clear(); pixels.show(); }
    statusStorage[0][0] = touchStatusBuffer[0]; statusStorage[0][1] = touchStatusBuffer[1];
}

void sendSPI(uint32_t data, uint8_t latchPin, uint8_t dataLength) {
    SPI1.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(latchPin, LOW);
    switch (dataLength) {  // 1~4 or 8, 16, 24, 
        case 1: case 8:
            SPI1.transfer(data & 0xFF);
            break;
        case 2: case 16:
            SPI1.transfer16(data & 0xFFFF);
            break;
        case 3: case 24:
            SPI1.transfer((data >> 16) & 0xFF);
            SPI1.transfer16(data & 0xFFFF);
            break;
        case 4: case 32:
            SPI1.transfer16((data >> 16) & 0xFFFF);
            SPI1.transfer16(data & 0xFFFF);
            break;
        default: 
            break;
    }
    digitalWrite(latchPin, HIGH);
    SPI1.endTransaction();
}

void setup() {
    Serial.begin(250000);
    pixels.begin(); pixels.setBrightness(10);
    
    Wire.setSDA(0); Wire.setSCL(1);
    Wire.begin();   
    Wire.setClock(400000);  // 400kHz I²C fast mode

    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    
    if (digitalRead(7))
    {
        uint16_t sysTime = micros();
        while (!Serial && micros() - sysTime < 2000000) { ; }  // wait for serial port to connect. timeout 2s
        if (micros() - sysTime > 2000000)  // if the reason run through here is not serial connection
        {
            pixels.setPixelColor(0, pixels.Color(0, 255, 255)); pixels.show();
            delay(200); // at least show for 200ms           // show onboard sky blue light
        }
    }
    
    Wire.beginTransmission(I2C_ADDRESS);
    if (Wire.endTransmission() == 0) Serial.println("IC 1 found");
    Wire.beginTransmission(I2C_ADDRESS + 1);
    if (Wire.endTransmission() == 0) Serial.println("IC 2 found");

    SPI1.setSCK(10); SPI1.setTX(11);
    SPI1.begin();
    pinMode(12, OUTPUT);

    if(!digitalRead(8)) KB.begin();

    for (int i = 0; i <= 16; i++) {
        uint16_t marqueeData = (1 << i);
        sendSPI(marqueeData, 12, 2);
        delay(100);
    }

    Serial.println("Start Program");
}

char keymap[2][2][8] = {  //   0    1    2    3    4    5    6    7
                        {   { '0', '0', '0', '0', 'r', 'r', 'e', 'e' } , // IC 1
                            { 'o', 'o', 'i', 'i', 'u', 'u', 't', 't' }   },
                        {   { '0', '0', '0', '0', '0', '0', '0', '0' } , // IC 2
                            { '0', '0', '0', '0', '0', '0', '0', '0' }   } };
uint8_t lastStatus[2][2] = { { 0, 0 }, { 0, 0 } }; // 2 ICs

unsigned long lastmicroS = micros();

void loop() {
    uint8_t touchStatus[2][2];
    requestTouchStatus(touchStatus);

    if(digitalRead(7) == LOW)
    {
        Serial.print("|");
        for (uint8_t re = 0; re < 2; re++)
        {   for (int pointer = 0; pointer < 8; pointer++)
            {
                uint8_t pressed = touchStatus[0][re] >> pointer & 0b00000001;
                Serial.print((char)(pressed ? (pointer+'0') : '-'));
                Serial.print("|");
            }
            if (!re) Serial.print(" |"); else Serial.println();
        }
    }
    
    if (digitalRead(8) == LOW && (lastStatus[0][0] != touchStatus[0][0] || lastStatus[0][1] != touchStatus[0][1]))
    {   for (uint8_t bfrsel = 0; bfrsel <= 1; bfrsel++)
        {   for (int pointer = 0; pointer < 8; pointer++)
            {   uint8_t pressed = touchStatus[0][bfrsel] >> pointer & 0b00000001,
                    lastPressed = lastStatus[0][bfrsel]  >> pointer & 0b00000001;
                if(pressed && (pressed != lastPressed))
                {
                    KB.add(keymap[0][bfrsel][pointer]);
                    if(!digitalRead(7)) Serial.println(keymap[0][bfrsel][pointer]);
                }
                else {
                    if(pressed != lastPressed)
                    {
                        KB.release(keymap[0][bfrsel][pointer]);
                        if(!digitalRead(7)) { Serial.print("-"); Serial.println(keymap[0][bfrsel][pointer]); }
                    }
                }
            }  // end for in byte
        }  // end for in buffer
    }  // end if

    KB.send();
    lastStatus[0][0] = touchStatus[0][0]; lastStatus[0][1] = touchStatus[0][1];
	Serial.print(micros() - lastmicroS);
	lastmicroS = micros();
}