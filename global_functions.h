//#define debug_flag                  // Comment out to suppress outputs being sent over serial.

// Timings
#define refresh_interval 1000/40    // [ms] Used for calibration and free play
#define highscore_disp_timeout 3000 // [ms] Time to display highscore before switching to the menu.
#define click_window_ms 1000        // [ms] Max delay between clicks in a double click

// Debug header definition
#ifdef debug_flag
	#define debug(str)       Serial.print   (str);
	#define debug_dec(num)   Serial.print   (num, DEC)
  #define debugln_dec(num) Serial.println (num, DEC)
	#define debug_bin(num)   Serial.print   (num,BIN)
	#define debugln(str)     Serial.println (str)
	#else
	#define debug(str) 
	#define debug_dec(num) 
	#define debug_bin(num) 
	#define debugln(str) 
#endif

// Count an array's length
#define mylength(array) ((unsigned int) (sizeof (array) / sizeof (array [0])))

bool game_over; // This is the break condition of while loops within the various games. False returns to menu.
byte level = 0; // Level variable used for all games
uint16_t N_times_played;

char receivedChar; // Serial port incoming
bool free_play = false; // AKA toddler mode

// EEPROM Stuff                                  // Length (bytes)
#define isMute_addr 0                            //      1 
#define memory_highscore_addr 1                  //      1
#define reaction_highscore_addr 2                //      1
#define N_times_played_addr 3                    //      2
#define reaction_initial_timeout_addr 5          //      2
#define memory_letters_delay_ms_addr  7          //      2

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
	if (      measured_val > 1000                   ){ return(6); } // Nothing pressed
    if ( abs( measured_val - red_val    ) < val_tol ){ return(0); }
	if ( abs( measured_val - blue_val   ) < val_tol ){ return(1); }
	if ( abs( measured_val - yellow_val ) < val_tol ){ return(2); }
	if ( abs( measured_val - green_val  ) < val_tol ){ return(3); }
	if (  measured_val                ==  white_val ){ return(4); } // Menu sits on a separate pin
	if ( abs( measured_val - black_val  ) < val_tol ){ return(5); }
	return(7); // Error measuring - ignore.
}

float average(uint16_t *array)  // assuming array is uint16_t, length is variable, ignore entries equal 5000.
{
  int len=0;
  long sum = 0L ;  // sum will be larger than an item, long for safety.
  for (int i = 0 ; i < N_avg ; i++){
  	if (array[i] != 5000) {sum += array[i]; len++;} // 5000 defines an outlier has been found at this index.
  }
  return  (uint16_t) ((float)sum / len);  // average will floored
}

uint16_t read_analog(){
	uint16_t measured_vals[N_avg];
	float avg_val;

	if (analogRead(menu_pin) < 1000){ // Pressed white (menu)!
		return( white_val );
	}
	
	float measured_val = 0;
	for (int i = 0; i < N_avg; i++){ // Try overcoming jitter
		measured_vals[i] = analogRead(button_pin);
    	delay(1);
	}
	avg_val = average(measured_vals);
  // Remove outliers
  	for (int i = 0; i < N_avg; i++){
		if( abs(measured_vals[i] - avg_val) > (3 * val_tol) ) { measured_vals[i] = 5000; }
	}
  measured_val = average(measured_vals);
  
  #ifdef debug_flag
    if (measured_val < 1000) {debugln_dec(measured_val);}
  #endif
	return( measured_val );
}

byte read_buttons(){
	// Output:
  // 7 Measurement error - ignore
	// 6 : nothing's pressed
	// 0-3 : R, B, Y, G Buttons
	// 4 : White
	// 5 : Black
	byte intermediate_button_state = which_button_pressed(read_analog());
	
	if (intermediate_button_state <=5) {// Debouncing function
		button_state = intermediate_button_state;
    if (which_button_pressed(read_analog()) == button_state){ // Validate its not jitter
  		if (intermediate_button_state <=3) {// Play the respective button's tone. Play an indefinite tone but stop it once button was released.
			  digitalWrite(RED_LED+button_state,HIGH);
			  PlayColor(button_state, 1000);
      }
      debugln(button2str[button_state] + "!");
    }
    else { // Jitter?
        button_state = 6;
    }
		while (intermediate_button_state != 6 ){
			intermediate_button_state = which_button_pressed(read_analog());
		}
	}
	else { // Pressed nothing.
		button_state = 6;
	}
	turn_off_LEDs();
	player_1.stop();
	player_2.stop();
	
	return(button_state);
}	

bool is_double_click(byte first_click){
	long t_start = millis();
	button_state = 6;
	while ((button_state==6) && ((millis()-t_start)<click_window_ms)){
		button_state = read_buttons();
	}
	return(button_state == first_click);
}

void end_game(){
	button_state = 5; // Signal the main sketch to go to the menu
	N_times_played++;
	EEPROM.put(N_times_played_addr,N_times_played);
}

void new_high_score(int level){
	const String str1 PROGMEM = "NEW HIGH SCORE: ";
	lcd_print(0,str1);
	lcd_print(1,String(level));
	COIN(level);
	flash_LEDs(3);
	FLAGPOLEFANFARE();
	long time = millis();
	while ((read_buttons()==6) && ((millis()-time) < 2*highscore_disp_timeout)) { delay(refresh_interval); }
	end_game();
}

void show_high_score(int level, byte highscore_addr){
	const String str1 PROGMEM = "Game over ;)";
	const String str2 PROGMEM = "Level ";
	const String str3 PROGMEM = "High score:";
	lcd_print(0,str1); lcd_print(1,str2 + String(level));
	DEATH();
	lcd_print(0,str3); lcd_print(1,String(EEPROM.read(highscore_addr)));
	GAMEOVER();
	long time = millis();
	while ((read_buttons()==6) && ((millis()-time) < 1*highscore_disp_timeout)){delay(refresh_interval);}
	end_game();
}	

void toggle_volume(){
	//lcd.scrollDisplayLeft();
	lcd_print(1,""); lcd.write(byte(6)); // Speaker character 
	if (isMute){
		isMute = false;
		lcd.write("v");
	}else
	{
		isMute = true;
		lcd.write("x");
	}
	debugln("Mute = " + String(isMute));
	EEPROM.put(isMute_addr, isMute);
}

void clear_EEPROM(){
	const String str1 PROGMEM = "2X click black";
	const String str2 PROGMEM = "clear EEPROM";
	const String str3 PROGMEM = "Done!";
	
	button_state = 6;
	lcd_print(0,str1);
	lcd_print(1,str2);
	while (button_state == 6){
		button_state = read_buttons();
		delay(refresh_interval);
	}
	if (button_state == 5){ // First click on white
		if (is_double_click(5)){
			// Clear stuff
			EEPROM.put(memory_highscore_addr, byte(0));
			EEPROM.put(reaction_highscore_addr, byte(0));
			N_times_played = uint16_t(0);
			EEPROM.put(N_times_played_addr, N_times_played);
			lcd.clear();
			lcd_print(0,str3);
			delay(750);
		}
	}
}	
