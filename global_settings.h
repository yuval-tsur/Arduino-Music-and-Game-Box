#define debug_flag // Comment out to suppress outputs being sent over serial.

#define refresh_interval 1000/15 // [ms] Used for reading button states
#define button_press_window 400 // [ms] once button press detected - stop listening so a long press isn't interpreted as several presses

// Awesome calibration table:
//      Button       Analog Value
#define red_val           42 
#define yellow_val       245               
#define green_val        378
#define blue_val         465

#define val_tol           50 // +- how many LSB can the measurement be far from the calibration

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

bool game_over = true; // This is the break condition of while loops within the various games. False returns to menu.
int button_state = 6;

char receivedChar;
bool free_play = false;

word InMemory[2] = {0,0}; // For Store in EEPROM - InMemory[1] is the current high score
int eeAddress = 0;

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

int read_buttons(){
	String print_str = String();
	uint16_t measured_val;
	uint16_t measured_val2;
	button_state = 6;
	// Output:
	// 6 : nothing's pressed
	// 0-3 : R, Y, G, B Buttons
	// 4 : OK (Not yet implemented)
	// 5 : Cancel (Not yet implemented)
	measured_val = analogRead(button_pin);
	delay(20);
	measured_val2 = analogRead(button_pin);
	measured_val = (measured_val2 > measured_val) ? measured_val : measured_val2; // Try overcoming jitter
	/* 		if (abs(measured_val-measured_val2) > 10){
		debugln(String(measured_val) + " , " + String(measured_val2));
	} */
	
	// Apply the calibration table
	if ( abs( measured_val - red_val ) < val_tol ){ print_str = "Red"; button_state = 0;}
	else if ( abs( measured_val - yellow_val ) < val_tol ){ print_str = "Yellow"; button_state = 1;}
	else if ( abs( measured_val - green_val ) < val_tol ){ print_str = "Green"; button_state = 2;}
	else if ( abs( measured_val - blue_val ) < val_tol ){ print_str = "Blue"; button_state = 3;}
	
	if (button_state>=0 & button_state<=3){
		digitalWrite(RED_LED+button_state,HIGH);
		debugln(print_str + "!!");
		// Play the respective button's tone
		PlayColor(button_state, 2);
	} else {turn_off_LEDs();}
	
	//debugln(measured_val);
	return(button_state);
}	