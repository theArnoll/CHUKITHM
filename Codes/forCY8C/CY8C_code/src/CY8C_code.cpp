#include <CypressCY8CMBR3116.h>
#include <Wire.h>
#include "NKRO_TinyUSB.h" // TinyUSB NKRO
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include "hardware/pwm.h" // RP2040 Hardware PWM

#define elif else if

#define I2C_ADDRESS 0x51     //I2C Address of the Cypress IC (0x37 is default)
#define REQUEST_TIMEOUT 40   //After how many request is a Timeout triggered

CY8CMBR3116 touchIC(I2C_ADDRESS, REQUEST_TIMEOUT);
CY8CMBR3116 touchIC2(I2C_ADDRESS + 1, REQUEST_TIMEOUT);
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
    uint8_t touchStatusBuffer[2][2];

    uint8_t error = touchIC.get_BUTTON_STAT(touchStatusBuffer[0]);
    if (error != 0) { Serial.print("IC1 Request Touch Status Loop: "); printError(error); }
                else { pixels.clear(); pixels.show(); }

    error = touchIC2.get_BUTTON_STAT(touchStatusBuffer[1]);
    if (error != 0) { Serial.print("IC2 Request Touch Status Loop: "); printError(error); }
                else { pixels.clear(); pixels.show(); }

    for (uint8_t a = 0; a <= 1; a++)
    {
        for (uint8_t b = 0; b <= 1; b++)
            statusStorage[a][b] = touchStatusBuffer[a][b];
    }
}

void sendSPI(uint32_t data, uint8_t latchPin, uint8_t dataLength) {
    SPI1.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(latchPin, LOW);
    switch (dataLength) {  
        case 1: case 8: SPI1.transfer(data & 0xFF); break;
        case 2: case 16: SPI1.transfer16(data & 0xFFFF); break;
        case 3: case 24: SPI1.transfer((data >> 16) & 0xFF); SPI1.transfer16(data & 0xFFFF); break;
        case 4: case 32: SPI1.transfer16((data >> 16) & 0xFFFF); SPI1.transfer16(data & 0xFFFF); break;
        default: break;
    }
    digitalWrite(latchPin, HIGH);
    SPI1.endTransaction();
}

#define CATHODE_TOP_ON    (1UL << 23) 
#define CATHODE_BOTTOM_ON (1UL << 22) 
#define CATHODE_BOTH_ON   0UL         

void runSingleDotMarquee(uint8_t delayTime) {
    for (int i = 0; i < 20; i++) {
        uint32_t marqueeData = (1UL << i); 
        marqueeData |= CATHODE_TOP_ON;     
        sendSPI(marqueeData, 12, 3);
        delay(delayTime);
    }
    for (int i = 0; i < 20; i++) {
        uint32_t marqueeData = (1UL << i); 
        marqueeData |= CATHODE_BOTTOM_ON;  
        sendSPI(marqueeData, 12, 3);
        delay(delayTime);
    }
}

void runDualBarGraph(uint8_t delayTime) {
    for (int i = 0; i < 20; i++) {
        uint32_t marqueeData = 0;
        for (int j = 0; j <= i; j++) marqueeData |= (1UL << j);     
        marqueeData |= CATHODE_BOTH_ON;    
        sendSPI(marqueeData, 12, 3); delay(delayTime);
    }
    for (int i = 19; i >= 0; i--) {
        uint32_t marqueeData = 0;
        for (int j = 0; j < i; j++) marqueeData |= (1UL << j);
        marqueeData |= CATHODE_BOTH_ON;
        sendSPI(marqueeData, 12, 3); delay(delayTime);
    }
}

void setup() {
    Serial.begin(250000);
    pixels.begin(); pixels.setBrightness(10);
    
    Wire.setSDA(0); Wire.setSCL(1);
    Wire.begin();   
    Wire.setClock(400000);  

    // Wait for IC to wake up
    delay(50);

    pinMode(15, INPUT_PULLUP); //7
    pinMode(14, INPUT_PULLUP); //8
    
    if (digitalReadFast(15))
    {
        uint32_t sysTime = millis();
        while (!Serial && millis() - sysTime < 2000) { delay(1); }  
        if (millis() - sysTime >= 2000)  
        {
            pixels.setPixelColor(0, pixels.Color(0, 255, 255)); pixels.show(); // Sky blue  
            delay(200); 
        }
    }
    
    Wire.beginTransmission(I2C_ADDRESS);
    if (Wire.endTransmission() == 0) Serial.println("IC 1 found");
    Wire.beginTransmission(I2C_ADDRESS + 1);
    if (Wire.endTransmission() == 0) Serial.println("IC 2 found");

    SPI1.setSCK(10); SPI1.setTX(11);
    SPI1.begin();
    pinMode(12, OUTPUT);

    // TODO: SW toggle Keyboard output
    KB.begin();

    runSingleDotMarquee(50);
    delay(100); 
    runDualBarGraph(50);

    // IR Pins
    for (uint8_t re = 2; re <= 7; re++)  // TSSP Pins
        pinMode(re, INPUT_PULLUP);

    gpio_set_function(8, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(8);  // Get Hardware PWM slice number
    pwm_set_clkdiv(slice_num, 125.0f);          // Set clock to 1MHz (125MHz / 125)
    pwm_set_wrap(slice_num, 26);                // Set frequency to ~38.4kHz (1MHz / 26)
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(8), 13);  // Set duty cycle to 2/26 (=1/13)
    pwm_set_enabled(slice_num, true);           // Start Hardware PWM

    digitalWrite(14, LOW);
    Serial.println("Start Program");
}

char keymap[2][2][8] = {  //   0    1    2    3    4    5    6    7
                        {   { '1', 'q', 'a', 'z', 'v', 'f', 'r', '4' } , // IC 1
                            { 'c', 'd', 'e', '3', 'x', 's', 'w', '2' }   },
                        {   { '5', 't', 'g', 'b', ',', 'k', 'i', '8' } , // IC 2
                            { 'm', 'j', 'u', '7', 'n', 'h', 'y', '6' }   } };
uint8_t lastStatus[2][2] = { { 0, 0 }, { 0, 0 } }; // 2 ICs
char IRkeymap[6] = { '.', 'l', 'o', '9', 'p', '0' };
bool lastIRstatus[6] = { 0 };

unsigned long lastmicroS = micros();

void loop() {
    uint8_t touchStatus[2][2];
    bool IRstatus[6];
    requestTouchStatus(touchStatus);

    bool needToSend = false;

    //  Serial print status
    for (uint8_t ICsel = 0; ICsel <= 1; ICsel++)
    {   if(digitalReadFast(15) == LOW)
        {
            Serial.print(ICsel + 1); Serial.print(": |");
            for (uint8_t re = 0; re < 2; re++)
            {   for (int pointer = 0; pointer < 8; pointer++)
                {
                    uint8_t pressed = touchStatus[ICsel][re] >> pointer & 0b00000001;
                    Serial.print((char)(pressed ? (pointer+'0') : '-'));
                    Serial.print("|");
                }
                if (!re) Serial.print(" |"); else Serial.println();
            }
        }
    }
    
    // Keyboard events
    for (uint8_t ICsel = 0; ICsel <= 1; ICsel++)
    {   if (digitalReadFast(14) == LOW && (lastStatus[ICsel][0] != touchStatus[ICsel][0] || lastStatus[ICsel][1] != touchStatus[ICsel][1]))
        {   for (uint8_t bfrsel = 0; bfrsel <= 1; bfrsel++)
            {   for (int pointer = 0; pointer < 8; pointer++)
                {   uint8_t pressed = touchStatus[ICsel][bfrsel] >> pointer & 0b00000001,
                        lastPressed = lastStatus[ICsel][bfrsel]  >> pointer & 0b00000001;
                    
                    if(pressed && (pressed != lastPressed))
                    {
                        KB.add(keymap[ICsel][bfrsel][pointer]);
                        needToSend = true;
                        if(!digitalReadFast(15)) Serial.println(keymap[ICsel][bfrsel][pointer]);
                    }
                    else {
                        if(pressed != lastPressed)
                        {
                            KB.release(keymap[ICsel][bfrsel][pointer]);
                            needToSend = true;
                            if(!digitalReadFast(15)) { Serial.print("-"); Serial.println(keymap[ICsel][bfrsel][pointer]); }
                        }
                    }
                }  // end for in byte
            }  // end for in buffer
        }  // end if
    } // end for

    for (uint8_t re = 0; re < 6; re++)
    {
        IRstatus[re] = digitalReadFast(2 + re);

        if (IRstatus[re] != lastIRstatus[re]) 
        {                                      //  // * in TSSP, 0 = blocked and no IR input, 1 = Detected IR
            if (lastIRstatus[re] > IRstatus[re]) // 1 -> 0
                KB.release(IRkeymap[re]);
            elif (lastIRstatus[re] < IRstatus[re]) // 0 -> 1
                KB.add(IRkeymap[re]);
            needToSend = true;
        }
    }

    // if (digitalReadFast(15) == LOW)
    // {
        Serial.print("IR State: | ");
        for (uint8_t re = 0; re < 6; re++)
        {
            Serial.print((int)IRstatus[re]);
            Serial.print(" | ");
        }
        Serial.println();
    // }

    if (needToSend) KB.send();

    for(uint8_t ICsel = 0; ICsel <= 1; ICsel++)
    {   for(uint8_t reg = 0; reg <= 1; reg++)
            lastStatus[ICsel][reg] = touchStatus[ICsel][reg];
    }
    for(uint8_t re = 0; re < 6; re++)
        lastIRstatus[re] = IRstatus[re];

	if (!digitalReadFast(15))
    {   Serial.println(micros() - lastmicroS);
        lastmicroS = micros();
    }
}