#define debug_flag // Comment out to suppress outputs being sent over serial.

#define refresh_interval 1000/15 // [ms] Used for reading button states
#define button_press_window 400 // [ms] Once button press detected - stop listening so a long press isn't interpreted as several presses

// Awesome calibration table:
//      Button       Analog Value
#define red_val     42 
#define yellow_val 245               
#define green_val  378
#define blue_val   465

#define val_tol     30 // +- how many LSB can the measurement be far from the calibration

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

#define floating_pin A0

String button2str[] = {"Red", "Yellow", "Green", "Blue"};

bool game_over = true; // This is the break condition of while loops within the various games. False returns to menu.
int button_state = 6;

char receivedChar;
bool free_play = false;

word InMemory[3] = {0,0}; // For Store in EEPROM - InMemory[0] is the current high Simon score, InMemory[1] is the current high Reaction score.
int eeAddress = 0;

byte Speaker[] = { // Sound character for the muting option (https://www.makerguides.com/character-lcd-arduino-tutorial/)
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

void flash_LEDs(){
	for(int i=1; i<=2; i++){
		turn_on_LEDs();
		delay(150);
		turn_off_LEDs();
		delay(150);
	}
}

byte which_button_pressed(int measured_val){
	// Apply the calibration table
	if ( abs( measured_val - red_val    ) < val_tol ){ return(0); }
	if ( abs( measured_val - yellow_val ) < val_tol ){ return(1); }
	if ( abs( measured_val - green_val  ) < val_tol ){ return(2); }
	if ( abs( measured_val - blue_val   ) < val_tol ){ return(3); }
	return(6);
}

uint16_t read_analog(){
	uint16_t measured_val = analogRead(button_pin);
	delay(20);
	return(min(measured_val,analogRead(button_pin))); // Try overcoming jitter
}

byte read_buttons(){
	byte intermediate_button_state;
	button_state = 6;
	// Output:
	// 6 : nothing's pressed
	// 0-3 : R, Y, G, B Buttons
	// 4 : OK (Not yet implemented)
	// 5 : Cancel (Not yet implemented)
	
	intermediate_button_state = which_button_pressed(read_analog());
	if (intermediate_button_state <=3) {
		button_state = intermediate_button_state;
		digitalWrite(RED_LED+button_state,HIGH);
		// Play the respective button's tone. Play an indefinite tone but stop it once button was released.
		PlayColor(button_state, 1000);
		debugln(button2str[button_state] + "!");
		while (intermediate_button_state <= 3){
			intermediate_button_state = which_button_pressed(read_analog());
		}
	}
	turn_off_LEDs();
	player_1.stop();
	player_2.stop();
	
	return(button_state);
}	

int random_seed(){
	return(analogRead(floating_pin));
}	

void toggle_volume(){
	//lcd.scrollDisplayLeft();
	lcd.home();
	lcd.write(byte(0));
	if (isMute){
		isMute = false;
		lcd.write("v");
	}else
	{
		isMute = true;
		lcd.write("x");
	}
}	