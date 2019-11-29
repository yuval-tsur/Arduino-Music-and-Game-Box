// External libraries
#include <Wire.h> 
#include <Tone.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Internal headers
#include "global_settings.h"
#include "LED-Functions.h"
#include "memory_game.h"
#include "Tone-Functions.h"

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
	EEPROM.get(eeAddress, GameSound); // Get Sound Settings
	eeAddress += sizeof(bool);
	EEPROM.get(eeAddress, InMemory);
	// end EEPROM reading part
}

void loop(){
	
	
	if (Serial.available() > 0) {
		receivedChar = Serial.read();
		if (receivedChar == 'a'){ // add letter
			add_letter();
		}
		
		if (receivedChar == 'c'){ // continue
		}
		
		if (receivedChar == 'p'){ // play sequence!
			play_sequence();
		}
		
		if (receivedChar == 's'){ // start normal play!
			debugln("Good luck, Player 1!");
			flash_LEDs();
			flash_LEDs();
			memory_game();
		}
		
		if (receivedChar == 'f'){ // free button pressing
			debugln("Welcome to Free Play! Press buttons and have fun!");
			free_play = true;
		}	
	}
	if (free_play ){read_buttons();}
	delay(refresh_interval);	
}	