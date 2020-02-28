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
	
	randomSeed(random_seed());
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
		PlayColor(button_state, (float)letter_duration/150);
		
		//player_1.stop();
		//player_2.stop();
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
	if (level > InMemory[1]) { // NEW HIGH SCORE!
		InMemory[1] = level;
		lcd.clear(); lcd.print(F("NEW HIGH SCORE: ")); lcd.setCursor(0,1); lcd.print("       " + String(level) + "       ");
		eeAddress = 2*sizeof(bool);
		EEPROM.put(eeAddress, InMemory[1]);
		COIN(1);
		flash_LEDs(3);
		CASTLE();
	}
	else {
		debugln("Failed on letter " + String(current_letter) + ", level " + String(level) + "\n:()" );
		lcd.clear(); lcd.print(F("  Game over ;)  ")); lcd.setCursor(0,1); lcd.print(F("    Level ")); lcd.print(String(level));
		level = 0;
		DEATH();
		GAMEOVER();
		lcd.clear(); lcd.print(F("   High score:  ")); lcd.setCursor(0,1); lcd.print("    " + String(InMemory[1]) + "       ");
	}
	
}
