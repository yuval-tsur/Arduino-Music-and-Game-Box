#define debug_flag // Comment out to suppress outputs being sent over serial.

// Timings
#define refresh_interval 1000/40 // [ms] Used for reading button states
#define highscore_disp_timeout 2000 // [ms] Time to display highscore before switching to the menu.

// Awesome calibration table: (Old prototype ver)
/* //      Button   Analog Value [LSB]
	#define red_val        42 
	#define yellow_val    245               
	#define green_val     378
	#define blue_val      465 
String button2str[] = {"Red", "Yellow", "Green", "Blue"}; */

// Awesome calibration table: (Final)
//      Button   Analog Value [LSB]
#define white_val     592
#define red_val       537
#define blue_val      466
#define yellow_val    372   
#define black_val     311
#define green_val     233
String button2str[] = {"Red", "Blue", "Yellow", "Green", "White", "Black"};

#define val_tol     20 // +- How many LSB the measurement can be far from the calibrated value

// Debug header definition
#ifdef debug_flag
	#define debug(str)       Serial.print   (str);
	#define debug_dec(num)   Serial.print   (num, DEC)
	#define debug_bin(num)   Serial.print   (num,BIN)
	#define debugln(str)     Serial.println (str)
	#else
	#define debug(str) 
	#define debug_dec(num) 
	#define debug_bin(num) 
	#define debugln(str) 
#endif

// Count an array's length
#define length(array) ((unsigned int) (sizeof (array) / sizeof (array [0])))

bool game_over; // This is the break condition of while loops within the various games. False returns to menu.
int level = 0; // Level variable used for all games
int button_state = 6; // See read_buttons()

char receivedChar; // Serial port incoming
bool free_play = false; // AKA toddler mode

// EEPROM Stuff
/* unsigned int InMemory[2] = {0,0}; // For Store in EEPROM - InMemory[0] is the current high Simon score, InMemory[1] is the current high Reaction score.
	unsigned int eeAddress = 0;
	unsigned int N_times_played; // Stored @ EEPROM. Both for showing the players and for random seeding.
*/
#define isMute_addr 0
#define memory_highscore_addr 1
#define reaction_highscore_addr 2
#define N_times_played_addr 3


byte Speaker[] = { // LCD Sound character for the muting option (https://www.makerguides.com/character-lcd-arduino-tutorial/)
	B00001,
	B00011,
	B00101,
	B01001,
	B01001,
	B01011,
	B11011,
	B11000
};

void turn_off_LEDs(){
	for (int i=0; i<4; i++){digitalWrite(RED_LED+i,LOW);};
}

void turn_on_LEDs(){
	for (int i=0; i<4; i++){digitalWrite(RED_LED+i,HIGH);};
}

void flash_LEDs(int n=3){
	for(int i=1; i<=n; i++){
		turn_on_LEDs();
		lcd.setBacklight(LOW);
		delay(150);
		turn_off_LEDs();
		lcd.setBacklight(HIGH);
		delay(150);
	}
}

byte which_button_pressed(int measured_val){
	// Apply the calibration table
	if ( abs( measured_val - red_val    ) < val_tol ){ return(0); }
	if ( abs( measured_val - blue_val   ) < val_tol ){ return(1); }
	if ( abs( measured_val - yellow_val ) < val_tol ){ return(2); }
	if ( abs( measured_val - green_val  ) < val_tol ){ return(3); }
	if ( abs( measured_val - white_val  ) < val_tol ){ return(4); }
	if ( abs( measured_val - black_val  ) < val_tol ){ return(5); }
	return(6); // Nothing pressed.
}

uint16_t read_analog(){
	uint16_t measured_val = analogRead(button_pin);
	delay(20);
	return(min(measured_val,analogRead(button_pin))); // Try overcoming jitter
}

byte read_buttons(){
	byte intermediate_button_state;
	// Output:
	// 6 : nothing's pressed
	// 0-3 : R, B, Y, G Buttons
	// 4 : OK (White)
	// 5 : Cancel (Black)
	
	intermediate_button_state = which_button_pressed(read_analog());
	if (intermediate_button_state <=5) {// Debouncing function
		button_state = intermediate_button_state;
		if (intermediate_button_state <=3) {// Play the respective button's tone. Play an indefinite tone but stop it once button was released.
			digitalWrite(RED_LED+button_state,HIGH);
			PlayColor(button_state, 1000);
		}
		debugln(button2str[button_state] + "!");
		while (intermediate_button_state <= 5){
			intermediate_button_state = which_button_pressed(read_analog());
		}
	}
	else { // Pressed white, black or nothing - those do not play a tone.
		button_state = intermediate_button_state;
	}
	turn_off_LEDs();
	player_1.stop();
	player_2.stop();
	
	return(button_state);
}	

void end_game(){
	long tic = millis();
	while ( ((millis()-tic)<highscore_disp_timeout) && (read_buttons() != 5)  ){
		delay(refresh_interval);
	}
	button_state = 5; // Signal the main sketch to go to the menu
}

void new_high_score(int level){
	lcd.clear(); lcd.print(F("NEW HIGH SCORE: ")); lcd.setCursor(0,1); lcd.print("       " + String(level) + "       ");
	COIN(1);
	flash_LEDs(3);
	CASTLE();
	end_game();
}

void show_high_score(int level){
	lcd.clear(); lcd.print(F("  Game over ;)  ")); lcd.setCursor(0,1); lcd.print(F("    Level ")); lcd.print(String(level));
	DEATH();
	GAMEOVER();
	lcd.clear(); lcd.print(F("   High score:  ")); lcd.setCursor(0,1); lcd.print("    " + String(level) + "       ");
	end_game();
}	

void toggle_volume(){
	//lcd.scrollDisplayLeft();
	EEPROM.put(0, isMute);
	lcd.setCursor(0,1);
	lcd.write(byte(0));
	if (isMute){
		isMute = false;
		lcd.write("v");
	}else
	{
		isMute = true;
		lcd.write("x");
	}
	Serial.println("Mute = " + String(isMute));
	}			