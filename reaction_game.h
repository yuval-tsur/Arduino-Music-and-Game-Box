#define delay_min 500      // [ms] minimal delay between clicks
#define delay_max 5000     // [ms] maximal delay between clicks
#define letter_duration 400 // [ms] time to display LED to trigger a reaction
#define initial_timeout 3000 // [ms] Time the player has to press the correct button at level 1. See timeout_per_level()

int timeout_per_level(byte level){
	// This function determines the speed of the game
	// Output is in ms.
	// ToDo: modify to accept a few levels of difficulty
	return(round(initial_timeout/sqrt((float)level)));
}	

void reaction_game(){
	debugln("Good luck, Player 1!");
	game_over = false;
	lcd.clear(); lcd.print(F("      Go        ")); lcd.setCursor(0,1); lcd.print("Reaction Game!!!");
	level = 1;
	ONEUP();
	byte current_letter;
	unsigned long time; // Time since LED turned off.
	
	while(!game_over){
		delay(random(delay_max-delay_min)+delay_min);	
		current_letter = random(4);
		digitalWrite(RED_LED+current_letter,HIGH);
		PlayColor(current_letter, (float)letter_duration/150);
		
		digitalWrite(RED_LED+current_letter,LOW);
		
		time = millis();
		while (true){
			button_state = read_buttons();
			if (button_state<=3){
				if (button_state == current_letter){ // Correct button
					debugln("Correct! Level " + String(level) + " cleared!");
					lcd.clear(); lcd.print(F("    Took you    ")); lcd.setCursor(0,1); lcd.print("     " + String((int)(millis()-time)) + "ms    ");
					level++;
					} else { // Game over
					game_over = true;
				}
				break;				
			}
			if ( (millis() - time) > timeout_per_level(level)){ // Time out
				game_over = true;
				break;
			}
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