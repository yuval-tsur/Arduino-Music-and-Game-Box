#define max_level 128 // Must be a power of 2
#define play_letters_per_sec 2

byte sequence[max_level/4];

byte level = 0;
const byte sequence_theme = 1; // 1: pseudo random
// In the future - can be 2: "Mary had a little lamb" :)

byte current_letter_index = 0; // Stores the index of letter just pressed by the player

void play_sequence(){
	byte word;
	byte letter;
	debug("Level " + String(level) + ". Playing sequence (start -> end):");
	delay(1000/play_letters_per_sec);
	for (int i_letter=0; i_letter<level; i_letter++){
		if (i_letter%4 == 0){
			word = sequence[(int)(i_letter/4)];
			debug(" ");
		}
		// Play a word, starting from 2 LSBs
		letter = 0;
		letter = bitRead(word,1)<<1 | bitRead(word,0)<<0; 		
		digitalWrite(RED_LED+letter,HIGH);
		debug_dec(letter);
		word = word >> 2;
		delay(0.5*1000/play_letters_per_sec);
		digitalWrite(RED_LED+letter,LOW);
		delay(0.5*1000/play_letters_per_sec);		
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
	game_over = false;
	current_letter_index = 0;
	level = 0;	
	add_letter();
	play_sequence();
	
	while (!game_over){
		button_state = read_buttons();
		if (button_state>=0 & button_state<=3) {
			delay(0.5*button_press_window);
			turn_off_LEDs();
			delay(0.5*button_press_window);
			
			if (is_correct_letter(button_state)){
				// Player got the right letter. Check if it was the end of the sequence
				if (current_letter_index == level-1){
					debugln("Correct! Level " + String(level) + " cleared!");
					current_letter_index = 0;
					flash_LEDs(); // Satisfaction!
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
				game_over = true;
				debugln("Failed on letter " + String(current_letter_index) + ", level " + String(level) + "\n:()" );
				current_letter_index = 0;
				level = 0;
				turn_on_LEDs();
				delay(3000);
				turn_off_LEDs();// Failure!
			}
		}
		delay(refresh_interval);
	}
}