#define IR_TX_PIN 4  // 紅外線發射 (記得串聯 220 歐姆左右的限流電阻)
#define IR_RX_PIN 5  // 38kHz 接收 IC 的 OUT 腳

void setup() {
	// pinMode(IR_RX_PIN, INPUT);
	for (uint8_t re = 8; re <= 13; re++)
		pinMode(re, INPUT)
	Serial.begin(115200);
}

void loop() {
	tone(IR_TX_PIN, 38000); 
	
	delayMicroseconds(500); 

	int rx_state[6];
	for (uint8_t re = 8; re <= 13; re++)
		rx_state[re - 8] = digitalRead(re);

	for (uint8_t re = 8; re <= 13; re++)
		noTone(re);

	for (uint8_t re = 0; re < 6; re++)
	{
		Serial.print(rx_state[re]);
		Serial.print(" | ");
	}   Serial.println();
}