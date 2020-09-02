#define SCREEN_WIDTH 16
#define SCREEN_HEIGHT 2
#define LCD_COLS 80
LiquidCrystal_I2C  lcd(  0x3F  ,   2  ,   1  ,   0  ,   4  ,   5  ,   6  ,   7  ); // Address is usually 0x27 or 0x3F
#define BACKLIGHT_PIN 3 // Backlight pin within the LCD module

// Custom characters
byte Speaker[] = { // LCD Sound character for the muting option (https://www.makerguides.com/character-lcd-arduino-tutorial/)
	B00001,
	B00011,
	B00101,
	B01001,
	B01001,
	B01011,
	B11011,
	B11000
};

/* // Custom progress bar by skywodd
// https://www.carnetdumaker.net/articles/faire-une-barre-de-progression-avec-arduino-et-liquidcrystal/
byte DIV_0_OF_5 [ 8 ] = {
  B00000, 
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
}; // 0/5

byte DIV_1_OF_5 [ 8 ] = {
  B10000, 
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
}; // 1/5

byte DIV_2_OF_5 [ 8 ] = {
  B11000, 
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
}; // 2/5

byte DIV_3_OF_5 [ 8 ] = {
  B11100, 
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
}; // 3/5

byte DIV_4_OF_5 [ 8 ] = {
  B11110, 
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
}; // 4/5 */

byte DIV_5_OF_5 [ 8 ] = {
  B11111, 
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
}; // 5/5


/* 
* Configuration function of the LCD screen for the progress bar. 
* Use custom characters from 0 to 5. Use 6 for the speaker char. 
*/ 
void  setup_progressbar(){
  // Store custom characters in the LCD screen memory
/*   lcd.createChar ( 0 , DIV_0_OF_5);
  lcd.createChar ( 1 , DIV_1_OF_5);
  lcd.createChar ( 2 , DIV_2_OF_5);
  lcd.createChar ( 3 , DIV_3_OF_5);
  lcd.createChar ( 4 , DIV_4_OF_5); */
  lcd.createChar ( 5 , DIV_5_OF_5);
  lcd.createChar ( 6 , Speaker);
}

void lcd_print(byte row, String text){
	lcd.setCursor(0,row);
    lcd.print("                "); // Sorry for the hard coding here
	lcd.setCursor(max(0,(SCREEN_WIDTH - (int)text.length())/2),row);
	lcd.print(text);
}

/* 
* Function drawing the progress bar. 
* 
* @param percent The percentage to display. 
*/ 

/* void draw_progressbar(int percent){
  //Move the cursor to the second line
  lcd.setCursor (0,1);

  //Map the range (0 ~ 100) to the range (0 ~ SCREEN_WIDTH * 5) 
  byte nb_columns = map(percent, 0.0, 100.0, 0, SCREEN_WIDTH*5);

  // Draw each character of the line
  for (byte i=0; i<SCREEN_WIDTH; ++i) {

    // Depending on the number of columns remaining to display 
    if (nb_columns == 0 ) { // Empty box 
      lcd.write ((byte)0);

    } else  if (nb_columns >= 5 ) { // Full box 
      lcd.write (5);
      nb_columns -= 5;

    } else { // Last box not empty
      lcd.write(nb_columns);
      nb_columns = 0;
    }
  }
} */
