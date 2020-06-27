// LED pins (inside big buttons)
#define LED_BLUE 12
#define LED_RED 9
#define LED_YELLOW 10
#define LED_GREEN 11

//Init LED Array
bool status_leds[4] = {false, false, false, false};
// Define the order of the pins in the display so that they are ordered according to the universal convention [OPTIONAL]
byte pin_leds[4] = {LED_YELLOW, LED_GREEN, LED_RED, LED_BLUE};

int GetColorPin (byte LED_color) {
	for(int i=0; i<4; i++){
    if (pin_leds[i] == LED_color) {return i;}
  }
  return -1;
}

// Power On Button LEDs
void PowerOnButtonLED () {
  for(int i=0; i<4; i++) {
    digitalWrite(RED_LED+i, (status_leds[i]) ? HIGH : LOW);
	}
}

// Set Active Button LED
void SetButtonLED (byte LED_color, bool power = true) {
  int i = -1;
  i = GetColorPin(LED_color);
  if (i >= 0) {status_leds[i] = !status_leds[i];}
  if (power) { PowerOnButtonLED(); }
}


// Power Off Button LEDs
void PowerOffButtonLED () {
  for(int i=0; i<4; i++) { status_leds[i] = false; }
  PowerOnButtonLED();
}
