// Assuming succeeding pins R, Y, G, B  
#define RED_LED 6 // Pin number for the red LED

int tronca = 15; // This value is higher and the notes are separated from each other. [OPTIONAL]
// Input pullup pin for reading all 4 buttons
#define button_pin A3

// External libraries
#include <Tone.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Speaker stuff
// Define speaker pins (PWM~) [REQUIRED]
#define PIN_PLAYER_1 10
#define PIN_PLAYER_2 11
bool isMute = false; // Global sound flag
Tone player_1;
Tone player_2;

LiquidCrystal_I2C  lcd(  0x3F  ,   2  ,   1  ,   0  ,   4  ,   5  ,   6  ,   7  );
#define BACKLIGHT_PIN 3

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
	delay(1000);	
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
			EEPROM.put(0, isMute);
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