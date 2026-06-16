#define IR_TX_PIN 4  

unsigned long lastResetTime = 0;

void setup() {
	for (uint8_t re = 5; re <= 10; re++) {
		pinMode(re, INPUT);
	}
	
	pinMode(IR_TX_PIN, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(115200);
}

void loop() {
	tone(IR_TX_PIN, 38000); 
	
	// 保持極短的發射，減少能量累積
	delayMicroseconds(300); 

	int rx_state[6] = {0};
	for (uint8_t re = 5; re <= 10; re++) {
		rx_state[re - 5] = digitalRead(re);  
	}

	noTone(IR_TX_PIN);

	for (uint8_t i = 0; i < 6; i++) {
		Serial.print(rx_state[i]);
		Serial.print(" | ");
	}
	Serial.print("Booted ");
	Serial.print(millis());
	Serial.print(" ms");
	Serial.println();

	// 【最後的黑魔法：週期性長休法】
	unsigned long currentTime = millis();
	
	// 如果已經連續高頻率掃描了 25 毫秒
	if (currentTime - lastResetTime > 25) {
		// 強制給予 15 毫秒的超級長假，讓 AGC 徹底重置歸零！
		delay(15);
		lastResetTime = millis();
	} else {
		// 正常的高頻率掃描只休息 2 毫秒
		delay(2);
	}
}