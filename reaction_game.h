#define delay_min 1000      // [ms] minimal delay between clicks
#define delay_max 10000     // [ms] maximal delay between clicks
#define letter_duration 100 // [ms] time to display LED to trigger a reaction
void reaction_game(){
	
	randomSeed(random_seed());
	game_over=false;
	ONEUP();
	byte current_letter;
	
	while(!game_over){
		delay(random(delay_max-delay_min)+delay_min);	
		current_letter = random(4);
		digitalWrite(RED_LED+current_letter,HIGH);
		PlayColor(current_letter,1000);
		delay(letter_duration);
		player_1.stop();
		player_2.stop();
		digitalWrite(RED_LED+current_letter,LOW);
		
		// Start a timer, fuse with read_button() which should return how much time elapsed
		if (Serial.available() > 0) {return;}
		
	}
	
	
	
}