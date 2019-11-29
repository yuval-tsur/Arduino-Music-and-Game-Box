#define debug_flag // Comment out to suppress outputs being sent over serial.

#define refresh_interval 1000/15 // [ms] Used for reading button states
#define button_press_window 400 // [ms] once button press detected - stop listening so a long press isn't interpreted as several presses

// Assuming succeeding pins R, Y, G, B  
#define RED_LED 6 // Pin number for the red LED
// Define speaker pins (PWM~) [REQUIRED]
#define PIN_PLAYER_1 14
#define PIN_PLAYER_2 15
// Input pullup pin to read from all 4 buttons
#define button_pin 17


// Awesome calibration table:
//      Button       Analog Value
#define red_val           42 
#define yellow_val       250               
#define green_val        383
#define blue_val         473

#define val_tol           10 // +- how many LSB can the measurement be far from the calibration

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
int button_state = 0;

char receivedChar;
bool free_play = false;

//LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C  lcd(  0x27  ,   2  ,   1  ,   0  ,   4  ,   5  ,   6  ,   7  );

word InMemory[2] = {0,0}; // For Store in EEPROM - InMemory[1] is the current high score
int eeAddress = 0;

// Sound related definitions
bool GameSound = true; // Master sound switch
Tone player_1;
Tone player_2;

//int tempo; //remove if compiles ok.
int tronca = 15; // This value is higher and the notes are separated from each other. [OPTIONAL]

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
	uint16_t measured_val = 0;
	uint8_t button_state = 6;
	
	// Output:
	// 6 : nothing's pressed
	// 0-3 : R, Y, G, B Buttons
	// 4 : OK
	// 5 : Cancel
	measured_val = analogRead(button_pin);
	
	// Apply the calibration table
	if ( abs( measured_val - red_val ) < val_tol ){ print_str = "Red"; button_state = 0;}
	else if ( abs( measured_val - yellow_val ) < val_tol ){ print_str = "Yellow"; button_state = 1;}
	else if ( abs( measured_val - green_val ) < val_tol ){ print_str = "Green"; button_state = 2;}
	else if ( abs( measured_val - blue_val ) < val_tol ){ print_str = "Blue"; button_state = 3;}
	else {print_str = ""; button_state = 6; turn_off_LEDs();}	
	
	if (button_state>=0 & button_state<=3){
		digitalWrite(RED_LED+button_state,HIGH);
		debugln(print_str + "!!");		
	}
	
	//debugln(measured_val);
	return(button_state);
}