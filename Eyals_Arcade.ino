// Arduino Nano pin definitions
#define RED_LED 5 // Pin number for the red LED, Assuming succeeding pins R, B, Y, G
#define button_pin A1 // Input pullup pin for reading all 6 buttons
byte button_state = 6; // See read_buttons()

// Awesome calibration table: (Old prototype ver)
/* //      Button   Analog Value [LSB]
#define red_val        42 
#define yellow_val    245               
#define green_val     378
#define blue_val      465 
String button2str[] = {"Red", "Yellow", "Green", "Blue"}; */

// Awesome calibration table: (Final)
//      Button   Analog Value [LSB]
#define white_val     593
#define red_val       538
#define blue_val      467
#define yellow_val    373   
#define black_val     310
#define green_val     235
String button2str[] = {"Red", "Blue", "Yellow", "Green", "White", "Black"};
#define val_tol         8 // +- How many LSB the measurement can be far from the calibrated value
#define N_avg           15 // Number of analog reads to average for a button value - minimize jitter

// External libraries
#include <Tone.h> // https://github.com/bhagman/Tone/ (V1.0.0)
#include <LiquidCrystal_I2C.h> // https://github.com/fmalpartida/New-LiquidCrystal (V1.5.0)
#include <EEPROM.h> // Arduino built-in

// Speaker stuff
#define PIN_PLAYER_1 10
#define PIN_PLAYER_2 11
int tronca = 15; // Increasing this value separates notes from each other. [OPTIONAL]
bool isMute = false; // Global sound switch, stored @ EEPROM, address "isMute_addr".
Tone player_1; // Speaker 1 object
Tone player_2; // Speaker 2 object

// Internal headers
#include "lcd_functions.h"
#include "tone_functions.h"
#include "global_functions.h"
#include "memory_game.h"
#include "reaction_game.h"

void setup(){
	pinMode(button_pin,INPUT_PULLUP);
	
	for(int i=0; i<4; i++){
		pinMode(RED_LED+i,OUTPUT);
	}	
	lcd.begin(SCREEN_WIDTH,SCREEN_HEIGHT);
	// Switch on the backlight
	lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
	lcd.setBacklight(HIGH);
	setup_progressbar();	
	#if defined(debug_flag)
	lcd.home(); lcd.print(F("Waiting for")); lcd.setCursor(0,1); lcd.print(F("serial..."));
	Serial.begin(9600);
	delay(100);
	while(!Serial);
	Serial.println("Welcome to Eyal's Arcade!\nPlayer 1 Ready...");
	#endif
	
	lcd.home();
	lcd.print(F(" EYAL'S ARCADE! ")); 	lcd.setCursor(0,1);
	lcd.print(F("    READY?  ;)  "));
	
	// Load settings
	EEPROM.get(isMute_addr, isMute); // Get Sound settings from the last time
	EEPROM.get(N_times_played_addr, N_times_played);
	if (N_times_played == 65536) {N_times_played = 0;} // init value in EEPROM
	randomSeed(N_times_played);
	EEPROM.get(reaction_initial_timeout_addr, reaction_initial_timeout);
	if (reaction_initial_timeout == 65536) {reaction_initial_timeout = 3000;} // init value
	EEPROM.get(memory_letters_delay_ms_addr, memory_letters_delay_ms);
	if (memory_letters_delay_ms == 65536) {memory_letters_delay_ms = 250;} // init value
	
	// Init Speakers
	player_1.begin(PIN_PLAYER_1);
	player_2.begin(PIN_PLAYER_2);
	
	introduzione();
	delay(200);	
	memory_game();
}

void listen_serial(){
	if (Serial.available() > 0) {
		receivedChar = Serial.read();
		
		if (receivedChar == 'a'){ // Add a sequence letter
			add_letter();
		}
		
		if (receivedChar == 'p'){ // Play sequence!
			play_sequence();
		}
		
		if (receivedChar == 'i'){ // Reset EEPROM
			EEPROM.put(memory_highscore_addr, 0);
			EEPROM.put(reaction_highscore_addr, 0);
			lcd.clear(); lcd.print(F("   Highscore    ")); lcd.setCursor(0,1); lcd.print("    Cleared.    ");
			delay(1000);
			menu();
		}
		if (receivedChar == 'm'){ // Play all music
			playAllMusic();
		}
		
		if (receivedChar == 's'){ // Start memory_game play!
			free_play = false;
			memory_game();
		}
		
		if (receivedChar == 'r'){ // Start reaction_game play!
			free_play = false;
			reaction_game();
		}
		
		if (receivedChar == 'f'){ // Free button pressing
			debugln("Welcome to Free Play! Press buttons and have fun!");
			free_play = true;
		}	
		if (receivedChar == 'v'){ // Volume toggle			
			toggle_volume();
		}
		
		if (receivedChar == 'c'){ // Button calibration
			free_play = false;
			while (!Serial.available()){
				debugln(analogRead(button_pin));
				delay(refresh_interval);
			}
		}
	}
}

void menu(){
	const String menu_choices[] PROGMEM = {"Memory Game", "Reaction Game", "Free Play", "Music"};
	button_state = 6;
	int current_choice = 0;
	lcd.clear(); lcd.print(menu_choices[current_choice] + "?");
	while (true){
		button_state = read_buttons();
		if (button_state == 4){ // Change selection
			current_choice++;
			if (current_choice == mylength(menu_choices)){
				current_choice = 0;
			}
			lcd.clear(); lcd.print(menu_choices[current_choice] + "?");
		}
		if (button_state == 5){ // Execute selection
			break;
		}
		if (button_state == 0){ // Double click for settings (child proof)
			if (is_double_click(button_state)){	
				settings();
				lcd.clear(); lcd.print(menu_choices[current_choice] + "?");
			}
		}
		if (Serial.available() > 0) {return;}
	}
	
	// Execute!
	lcd.clear(); lcd.print("Execute!");
	
	switch (current_choice){
		case 0:
		free_play = false;
		memory_game();
		break;
		case 1:
		free_play = false;
		reaction_game();
		break;
		case 2:
		free_play = true;
		lcd.clear(); lcd.print(menu_choices[current_choice] + "!");
		break;
		case 3:
		playAllMusic();
		break;
	}
}

void display_parameter(byte current_setting){
	//{"Memory speed", "Reaction speed", "Times played", "Memory score", "Reaction score"}
	switch (current_setting){
		case 0:
		lcd_print(1, String(memory_letters_delay_ms));
		break;
		case 1:
		lcd_print(1, String(reaction_initial_timeout));	
		break;
		case 2:
		lcd_print(1, String(N_times_played));	
		break;
		case 3:
		lcd_print(1, String(EEPROM.read(memory_highscore_addr)));	
		break;
		case 4:
		lcd_print(1, String(EEPROM.read(reaction_highscore_addr)));
		break;
		case 5:
		lcd_print(1,"");
		lcd.write(byte(6));
		lcd.print((isMute) ? "x":"v");
		break;
	}
}

void change_up_setting(byte current_setting){
	switch (current_setting){
		case 0:
		memory_letters_delay_ms = min(2500,round(((float)memory_letters_delay_ms) * 1.15));
		break;
		case 1:
		reaction_initial_timeout = min(5000,round(((float)reaction_initial_timeout) * 1.15));
		break;
		case 5:
		toggle_volume();
		break;
		case 6:
		clear_EEPROM();
		menu();
		break;
	}
	display_parameter(current_setting);
}

void change_down_setting(byte current_setting){
	switch (current_setting){
		case 0:
		memory_letters_delay_ms = max(100,round(((float)memory_letters_delay_ms) / 1.15));
		break;
		case 1:
		reaction_initial_timeout = max(100,round(((float)reaction_initial_timeout) / 1.15));
		break;
		case 5:
		toggle_volume();
		break;
		case 6:
		clear_EEPROM();
		menu();
		break;
	}
	display_parameter(current_setting);	
}

void settings(){
	const String settings_choices[] PROGMEM = {"Memory [ms]", "Reaction [ms]", "Times played", "Memory score", "Reaction score", "Volume", "Clear EEPROM"};
	byte current_setting = 0;
	lcd_print(0, settings_choices[current_setting]);
	display_parameter(current_setting);
	while (true){
		button_state = read_buttons();
		if (button_state == 4){ // Change selection
			current_setting++;
			if (current_setting == mylength(settings_choices)){
				current_setting = 0;
			}
			lcd_print(0, settings_choices[current_setting]);
			display_parameter(current_setting);
		}
		if (button_state == 5){ // Save and quit this sub-menu
			EEPROM.put(memory_letters_delay_ms_addr,memory_letters_delay_ms);		
			EEPROM.put(reaction_initial_timeout_addr,reaction_initial_timeout);
			return;
		}
		if (button_state == 2){ // Mute/ Unmute
			toggle_volume();
		}
		if (button_state == 0){ // Change up
			change_up_setting(current_setting);
		}
		if (button_state == 1){ // Change down
			change_down_setting(current_setting);
		}
		if (Serial.available() > 0) {return;}
	}
}

void loop(){
	listen_serial();
	if ( (button_state == 4) || read_buttons() == 4){
		menu();
	}
	if (free_play){read_buttons();}
	delay(refresh_interval);	
}			
