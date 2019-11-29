// Assuming succeeding pins R, Y, G, B  
#define RED_LED 6 // Pin number for the red LED
// Define speaker pins (PWM~) [REQUIRED]
#define PIN_PLAYER_1 10
#define PIN_PLAYER_2 11
bool GameSound = true; // Master sound switch
//int tempo; //remove if compiles ok.
int tronca = 15; // This value is higher and the notes are separated from each other. [OPTIONAL]
// Input pullup pin for reading all 4 buttons
#define button_pin A3

// External libraries
#include <Wire.h> 
#include <Tone.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// External Hardware
Tone player_1;
Tone player_2;
//LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C  lcd(  0x27  ,   2  ,   1  ,   0  ,   4  ,   5  ,   6  ,   7  );

// Internal headers
#include "Tone-Functions.h"
#include "global_settings.h"
#include "LED-Functions.h"
#include "memory_game.h"

void setup(){
	pinMode(button_pin,INPUT_PULLUP);
	for(int i=0; i<4; i++){
		pinMode(RED_LED+i,OUTPUT);
	}	
	#if defined(debug_flag)
	Serial.begin(9600);
	delay(100);
	while(!Serial);                         // If entered debug mode, Arduino will wait here for the Serial monitor to come alive.
	Serial.println("Welcome to Eyal's Arcade!\nPlayer 1 Ready...");
	#endif
	randomSeed(millis()); // Perhaps replace with analog input of floating pin?
	// EEPROM reading part
	eeAddress = 0;
	//EEPROM.get(eeAddress, GameSound); // Get Sound Settings
	eeAddress += sizeof(bool);
	EEPROM.get(eeAddress, InMemory);
	// end EEPROM reading part
	
	// Init Speakers
	player_1.begin(PIN_PLAYER_1);
	player_2.begin(PIN_PLAYER_2);
	
	introduzione();
}

void loop(){
	
	
	if (Serial.available() > 0) {
		receivedChar = Serial.read();
		if (receivedChar == 'a'){ // Add a sequence letter
			add_letter();
		}
		
		if (receivedChar == 'p'){ // Play sequence!
			play_sequence();
		}
		
		if (receivedChar == 'i'){ // Play introduzione sound
			introduzione();
		}
		if (receivedChar == 'm'){ // Play all music
			playAllMusic();
		}
		
		if (receivedChar == 's'){ // Start normal play!
			debugln("Good luck, Player 1!");
			free_play = false;
			memory_game();
		}
		
		if (receivedChar == 'f'){ // Free button pressing
			debugln("Welcome to Free Play! Press buttons and have fun!");
			free_play = true;
		}	
		
		if (receivedChar == 'c'){ // Button calibration
			free_play = false;
			while (!Serial.available()){
				debugln(analogRead(button_pin));
				//delay(refresh_interval);
			}
		}
	}
	if (free_play ){read_buttons();}
	delay(refresh_interval);	
}	