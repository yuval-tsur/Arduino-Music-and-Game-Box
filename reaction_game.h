#define delay_min 750      // [ms] minimal delay between clicks
#define delay_max 5000     // [ms] maximal delay between clicks
#define letter_duration 400 // [ms] time to display LED to trigger a reaction
uint16_t reaction_initial_timeout; // [ms] Time the player has to press the correct button at level 1. See timeout_per_level(). Default value in setup()

int timeout_per_level(byte level){
	// This function determines the speed of the game
	// Output is in ms.
	return(round(reaction_initial_timeout/sqrt((float)level)));
}	

void LCD_block_row(){
	lcd.setCursor(0,1);
	for(byte i=0; i<SCREEN_WIDTH; i++){lcd.write(5);};
	lcd.setCursor(0,1); // For the progress bar	
}

void reaction_game(){
	debugln("Good luck, Player 1!");
	game_over = false;
	lcd.clear(); lcd.print(F("      Go        ")); lcd.setCursor(0,1); lcd.print(F("Reaction Game!!!"));
	level = 1;
	ONEUP();
	byte current_letter;
	unsigned long time; // Time since LED turned off.
	unsigned long progress_bar_updated; // Time elapsed since last progress bar update
	byte nb_cols = 0; // Progress bar value
	
	delay(delay_min);
	while(!game_over){
		LCD_block_row();
		delay(random(delay_max-delay_min)); // delay_min already spent in the level cleared part.	
		current_letter = random(4);
		digitalWrite(RED_LED+current_letter,HIGH);
		PlayColor(current_letter, (float)letter_duration/150);
		
		digitalWrite(RED_LED+current_letter,LOW);
		
		time = millis();
		progress_bar_updated = time;
		while (true){ // Wait here for the player's reaction
			read_buttons();
			if (button_state<=3){ // Check correct button was pressed
				if (button_state == current_letter){ // Correct button
					debugln("Correct! Level " + String(level) + " cleared!");
					lcd.clear(); lcd.print(F("    Took you    ")); lcd_print(1,String((int)(millis()-time)) + "ms");
					level++;
					delay(delay_min);
					} else { // Game over
					game_over = true;
				}
				break;				
			}
			// Update the progress bar
			if ( (millis() - progress_bar_updated) > (timeout_per_level(level)/(0.5*SCREEN_WIDTH))){ // Time to update progress bar
				nb_cols++;
				lcd.print("  "); 
				progress_bar_updated = millis();
			}
			
			//The following is redundant: please remove it when you get some time...
			if ( (millis() - time) > timeout_per_level(level)){ // Time out
				game_over = true;
				break;
			}
			//------
		}
		
		if (Serial.available() > 0) {return;}		
	}
	
	// After game over:
	if (level > EEPROM.read(reaction_highscore_addr)) { // NEW HIGH SCORE!
		EEPROM.put(reaction_highscore_addr, level);
		new_high_score(level);
	}
	else {
		debugln("Failed on letter " + String(current_letter) + ", level " + String(level) + "\n:()" );
		show_high_score(level,reaction_highscore_addr);
	}
}	