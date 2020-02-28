// Arduino Nano pin definitions
#define RED_LED 5 // Pin number for the red LED, Assuming succeeding pins R, B, Y, G
#define button_pin A1 // Input pullup pin for reading all 6 buttons
#define floating_pin A0 // For giving a random seed

// External libraries
#include <Tone.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Define speaker pins (PWM~) [REQUIRED]
#define PIN_PLAYER_1 10
#define PIN_PLAYER_2 11

// Speaker stuff
int tronca = 15; // Setting this value larger separates notes from each other. [OPTIONAL]
bool isMute = true; // Global sound switch
Tone player_1; // Speaker 1 object
Tone player_2; // Speaker 2 object

LiquidCrystal_I2C  lcd(  0x3F  ,   2  ,   1  ,   0  ,   4  ,   5  ,   6  ,   7  ); // Address is usually 0x27 or 0x3F
#define BACKLIGHT_PIN 3 // Backlight pin within the LCD module

// Internal headers
#include "Tone-Functions.h"
#include "global_settings.h"
#include "LED-Functions.h"
#include "memory_game.h"
#include "reaction_game.h"

void setup(){
	pinMode(button_pin,INPUT_PULLUP);
	pinMode(floating_pin,INPUT);
	
	for(int i=0; i<4; i++){
		pinMode(RED_LED+i,OUTPUT);
	}	
	lcd.begin(16,2);
	// Switch on the backlight
	lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
	lcd.setBacklight(HIGH);
	lcd.createChar(0, Speaker);
	
	#if defined(debug_flag)
	lcd.home();
	lcd.print(F("Waiting for"));
	lcd.setCursor(0,1);
	lcd.print(F("serial..."));
	Serial.begin(9600);
	delay(100);
	while(!Serial);                         // If entered debug mode, Arduino will wait here for the Serial monitor to come alive.
	Serial.println("Welcome to Eyal's Arcade!\nPlayer 1 Ready...");
	#endif
	
	lcd.home();
	lcd.print(F(" EYAL'S ARCADE! "));
	lcd.setCursor(0,1);
	lcd.print(F("    READY?  ;)  "));
	
	// EEPROM reading part
	eeAddress = 0;
	EEPROM.get(eeAddress, isMute); // Get Sound Settings
	eeAddress += sizeof(bool);
	EEPROM.get(eeAddress, InMemory);
	// end EEPROM reading part
	
	// Init Speakers
	player_1.begin(PIN_PLAYER_1);
	player_2.begin(PIN_PLAYER_2);
	
	introduzione();
	delay(750);	
	memory_game();
}

void loop(){
	
	listen_serial();
	if (free_play ){read_buttons();}
	delay(refresh_interval);	
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
			InMemory[0]=0; InMemory[1]=0; 
			EEPROM.put(sizeof(bool), 0);
			lcd.clear(); lcd.print(F("   Highscore    ")); lcd.setCursor(0,1); lcd.print("    Cleared.    ");
		}
		if (receivedChar == 'm'){ // Play all music
			playAllMusic();
		}
		
		if (receivedChar == 's'){ // Start memory_game play!
			debugln("Good luck, Player 1!");
			free_play = false;
			memory_game();
		}
		
		if (receivedChar == 'r'){ // Start reaction_game play!
			debugln("Good luck, Player 1!");
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
				//delay(refresh_interval);
			}
		}
	}
}