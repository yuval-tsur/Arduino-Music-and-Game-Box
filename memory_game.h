#define max_level 128 // Must be a power of 2
#define letters_delay_ms 300 

byte sequence[max_level/4];

byte level = 0;
const byte sequence_theme = 1; // 1: pseudo random
// In the future - can be 2: "Mary had a little lamb" :)

byte current_letter_index = 0; // Stores the index of letter just pressed by the player

void play_sequence(){
	byte word;
	byte letter;
	debug("Level " + String(level) + ". Playing sequence (start -> end):");
	delay(letters_delay_ms);
	for (int i_letter=0; i_letter<level; i_letter++){
		if (i_letter%4 == 0){
			word = sequence[(int)(i_letter/4)];
			debug(" ");
		}
		// Play a word, starting from 2 LSBs
		letter = 0;
		letter = bitRead(word,1)<<1 | bitRead(word,0)<<0; 		
		digitalWrite(RED_LED+letter,HIGH);
		// Play the respective button's tone
		PlayColor(letter, 2);
		while(player_1.isPlaying()){delay(5);}
		
		debug_dec(letter);
		word = word >> 2;
		digitalWrite(RED_LED+letter,LOW);
		delay(letters_delay_ms);		
	}	
	debugln("");
}

void add_letter(){
	byte letter;
	
	switch (sequence_theme){
		case 1: //1: pseudo random
		letter = random(4); // 0, 1, 2, 3 : R, Y, G, B, Respectively		
		break;
		
		case 2: //2: Mary had a little lamb - To be developed
		//letter = random(4); // 0, 1, 2, 3 : R, Y, G, B, Respectively		
		
		break;
	}
	// Write the addition to the sequence
	bitWrite(sequence[level/4], 2*(level % 4) + 0 , bitRead(letter,0));
	bitWrite(sequence[level/4], 2*(level % 4) + 1 , bitRead(letter,1));
	
	level++;	
	
	#if defined(debug_flag)
		Serial.print("Level " + String(level) + ". Added letter: " + String(letter) + ". Sequence (end -> start): ");
		for (int i=((level-1)/4); i>=0; i--){
			for (int j=7; j>=0; j--){
				Serial.print(bitRead(sequence[i],j));
			}
			Serial.print(" ");
		}
		Serial.println("");
	#endif
}

bool is_correct_letter(byte button_state){
	byte word = sequence[(int)(current_letter_index/4)];
	byte expected_letter = bitRead(word,2*(current_letter_index%4) + 1)<<1 | bitRead(word,2*(current_letter_index%4) +0)<<0; 		
	debug("Current word: ");
	debug_bin(word);
	debug(", Button expected: " + String(expected_letter) + ". Button pressed: " + String(button_state) + "\n");
	return button_state == expected_letter;
}

void memory_game(){
	randomSeed(random_seed());
	game_over = false;
	lcd.clear(); lcd.print(F("      Go        ")); lcd.setCursor(0,1); lcd.print(" Memory Game!!! ");
	ONEUP();	
	current_letter_index = 0;
	level = 0;	
	add_letter();
	play_sequence();
	
	while (!game_over){
		randomSeed(millis()); // Perhaps replace with analog input of floating pin?
		button_state = read_buttons();
		if (button_state<=3) {
			
			if (is_correct_letter(button_state)){
				// Player got the right letter. Check if it was the end of the sequence
				if (current_letter_index == level-1){
					debugln("Correct! Level " + String(level) + " cleared!");
					lcd.clear(); lcd.print("    Level " + String(level)); lcd.setCursor(0,1); lcd.print(F("   completed!   "));
					current_letter_index = 0;
					POWERUP();
					
					delay(400);					
					add_letter();
					play_sequence();
				}
				else{ // Player pressed the correct button, in the middle of the sequence
					current_letter_index++;
					debugln("Go on!!!");
				}
				turn_off_LEDs();
			}
			else{ // Player got the wrong letter
				if (level > InMemory[0]) { // NEW HIGH SCORE!
					InMemory[0] = level;
					lcd.clear(); lcd.print(F("NEW HIGH SCORE: ")); lcd.setCursor(0,1); lcd.print("       " + String(level) + "       ");
					eeAddress = sizeof(bool);
					EEPROM.put(eeAddress, InMemory[0]);
					COIN(1);
					flash_LEDs(3);
					CASTLE();
				}
				else {
					game_over = true;
					debugln("Failed on letter " + String(current_letter_index) + ", level " + String(level) + "\n:()" );
					lcd.clear(); lcd.print(F("  Game over ;)  ")); lcd.setCursor(0,1); lcd.print(F("    Level ")); lcd.print(String(level));
					current_letter_index = 0;
					level = 0;
					DEATH();
					GAMEOVER();
					lcd.clear(); lcd.print(F("   High score:  ")); lcd.setCursor(0,1); lcd.print("    " + String(InMemory[0]) + "       ");
					}
			}
		}
		if (Serial.available() > 0) {return;}
		delay(refresh_interval);
	}
}	