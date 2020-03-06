// Arduino Nano pin definitions
#define RED_LED 5 // Pin number for the red LED, Assuming succeeding pins R, B, Y, G
#define button_pin A1 // Input pullup pin for reading all 6 buttons
int button_state = 6; // See read_buttons()

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

// External libraries
#include <Tone.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Speaker stuff
#define PIN_PLAYER_1 10
#define PIN_PLAYER_2 11
int tronca = 15; // Setting this value larger separates notes from each other. [OPTIONAL]
bool isMute = true; // Global sound switch, stored @ EEPROM, address "isMute_addr".
Tone player_1; // Speaker 1 object
Tone player_2; // Speaker 2 object

// LCD stuff
LiquidCrystal_I2C  lcd(  0x3F  ,   2  ,   1  ,   0  ,   4  ,   5  ,   6  ,   7  ); // Address is usually 0x27 or 0x3F
#define BACKLIGHT_PIN 3 // Backlight pin within the LCD module

// Internal headers
#include "Tone-Functions.h"
#include "global_settings.h"
#include "memory_game.h"
#include "reaction_game.h"

void setup(){
	pinMode(button_pin,INPUT_PULLUP);
	
	for(int i=0; i<4; i++){
		pinMode(RED_LED+i,OUTPUT);
	}	
	lcd.begin(16,2);
	// Switch on the backlight
	lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
	lcd.setBacklight(HIGH);
	lcd.createChar(0, Speaker);
	
	#if defined(debug_flag)
	lcd.home(); lcd.print(F("Waiting for")); lcd.setCursor(0,1); lcd.print(F("serial..."));
	Serial.begin(9600);
	delay(100);
	while(!Serial); // If entered debug mode, Arduino will wait here for the Serial monitor to come alive.
	Serial.println("Welcome to Eyal's Arcade!\nPlayer 1 Ready...");
	#endif
	
	lcd.home();
	lcd.print(F(" EYAL'S ARCADE! ")); 	lcd.setCursor(0,1);
	lcd.print(F("    READY?  ;)  "));
	
	// Load settings
	EEPROM.get(isMute_addr, isMute); // Get Sound settings from the last time
	randomSeed(EEPROM.read(N_times_played_addr));
	
	// Init Speakers
	player_1.begin(PIN_PLAYER_1);
	player_2.begin(PIN_PLAYER_2);
	
	introduzione();
	delay(250);	
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
		if (button_state == 5){ // Change selection
			current_choice++;
			if (current_choice == length(menu_choices)){
				current_choice = 0;
			}
			lcd.clear(); lcd.print(menu_choices[current_choice] + "?");
		}
		if (button_state == 4){ // Execute selection
			break;
		}
		if (button_state == 2){ // Mute/ Unmute
			toggle_volume();
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

void loop(){
	listen_serial();
	if ( (button_state == 5) || read_buttons() == 5){
		menu();
	}
	if (free_play){read_buttons();}
	delay(refresh_interval);	
}	