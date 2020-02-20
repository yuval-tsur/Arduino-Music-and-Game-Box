// Easy to use notes (Do not modify)
#define SOL_d2 104
#define LA2    110
#define LA_d2  114
#define SI2    124
#define DO3    131
#define DO_d3  139
#define RE3    147
#define RE_d3  156
#define MI3    165
#define FA3    175
#define FA_d3  185
#define SOL3   196
#define SOL_d3 208
#define LA3    220
#define LA_d3  233
#define SI3    247
#define DO4    262
#define DO_d4  277
#define RE4    294
#define RE_d4  311
#define MI4    330
#define FA4    349
#define FA_d4  370
#define SOL4   392
#define SOL_d4 415
#define LA4    440
#define LA_d4  466
#define SI4    494
#define DO5    523
#define DO_d5  554
#define RE5    587
#define RE_d5  622
#define MI5    659
#define FA5    698
#define FA_d5  740
#define SOL5   784
#define SOL_d5 831
#define LA5    880
#define LA_d5  932
#define SI5    988
#define DO6    1046
#define DO_d6  1109
#define RE6    1175
#define RE_d6  1245
#define MI6    1319
#define FA6    1397
#define FA_d6  1480
#define SOL6   1568
#define SOL_d6 1661
#define LA6   1760
#define LA_d6  1865

// 4/3 (DON'T EDIT)
#define TERZINA 1.333

// 4/3/2 (DON'T EDIT)
#define TERZINA_2 0.666

// 1/3 (DON'T EDIT)
#define TERZINA_3 0.333

// 1/4 (DON'T EDIT)
#define SEMICROMA 0.25

// Assign Button and Notes
int ButtonNote[4] = {SOL_d4, RE_d4, FA_d4, DO_d4};

/*
    _____________________________________________________________
    |                          Uhm...                            |
    |              Cio potrebbe annoiarti a morte!               |
    |____________________________________________________________|
    Alcuni sanno che tutte le note hanno una frequenza e che essa
    si puo prevedere tramite questa funzione matematica:
	
	frequenza(n) = 440 * 2^(n/12)
	[ http://it.wikipedia.org/wiki/Nota_musicale ]
	
    Guardatela bene.
    Questa formula ci rivela che tutte le note che noi usiamo
    sono in riferimento ad una e una sola frequenza: 440Hz.
    Ovvero il nostro 'LA' ('A' in americano).
	
    Fermandosi a ragionare ci si rente conto di una cosa strana:
    Qualcuno si e mai chiesto per quale motivo noi 'normali' chiamiamo
    le note DO, RE MI FA... iniziando dal DO, mentre quegli 'strani' degli
    Inglesi le chiamano A, B, C, D... inziando dalla A che sarebbe un LA?
	
    Ebbene siamo noi quegli strani. Perche la notazione Inglese inizia
    appunto con la nota piu importante, ovvero quella a 440Hz: 'A'.
	
    Ed e sconvolgente sapere che se noi siamo quelli 'strani', la 'colpa' e tutta
    di un monaco (pure Italiano).
    
	[ http://it.wikipedia.org/wiki/Paolo_Diacono ]
	
    Curioso? No? Bah...
	
    Detto cio, c'e un motivo se siamo dovuti andare a cercare quella
    formula. Infatti per suonare io non faccio altro che passare una frequenza
    ad un nostro metodo 'suona(frequenza, tempo)'.
    Percio all'interno di 'suona', avrei potuto creare un enorme 'switch case',
    controllando manualmente tutte le frequenze possibili immaginabili che mi
    arrivano in ingresso e associarle a quello che devo far visualizzare sul display.
    Questo pero ci e sembrato un metodo di programmazione idiota, e quindi ci
    siamo presi una nottata del nostro tempo per esplicitare la nota dalla
    funzione della frequenza citata in alto, e abbiamo ottenuto un vero e proprio
    riconoscitore di note dinamico.
    
    Ed ecco il risultato:
*/


int inputTempo() {
	//global tempo_massimo, tempo_minimo;
	//return (analogRead(PIN_SPEED) / 1023.0) * (tempo_massimo - tempo_minimo) + tempo_minimo;
	return 150;
}

int getAmericanIndexOfLetterFromFrequency(int frequency) {
	double nota = 12*(log(((double)frequency)/440)/log(2));
	//Serial.println();
	//Serial.println(nota);
	while(nota >= 12) {
		nota = nota-12;
	}
	while(nota < 0 ) {
		nota = nota+12;
	}
	//Serial.println(nota);
	// Arrotondamento
	float virgola = nota - ((int) nota);
	if(virgola >= 0) {
		return (virgola < 0.5) ? nota : ++nota;
		} else {
		return (virgola <-0.5) ? nota : --nota;
	}
}

void setLetter(int index_letter, bool blink = true) {
	
	// needed for blinking lights during music
	boolean letter_table[12][4] = {
		{true, true, false, false}, // A
		{true, false, false, true}, // A_d
		{false, false, true, true}, // B
		{false, true, false, true}, // C_d
		{true, false, true, false}, // C
		{false, true, true, false}, // D
		
		{true, false, false, false}, // D_d
		{false, true, false, false}, // E
		{false, false, true, false}, // F
		{false, false, false, true}, // F_d
		
		{true, true, true, true}, // G
		{false, false, false, false} // G_d
	};
	if (blink == true)
	{
		for(int i=0; i<4; i++) {
			digitalWrite(RED_LED+i, (letter_table[index_letter][i]) ? HIGH : LOW);
		}
	}
}

void suona(int frequency, float fract, bool blink = true) {
	setLetter(getAmericanIndexOfLetterFromFrequency(frequency), blink);
	if (isMute) {frequency = 0;}
	player_2.play(frequency, inputTempo() * fract - tronca);
	//enableDisplay(DISPLAY_1);
	while(player_2.isPlaying());
	//disableDisplay(DISPLAY_1);
	delay(tronca);
	setLetter(11, blink); // turn off all LEDs
}

void suonaCoppia(int frequency, float fract, bool blink = true) {
	int index_letter = getAmericanIndexOfLetterFromFrequency(frequency);
	if (isMute) {frequency = 0;}
	int ms = inputTempo() * fract - tronca;
	player_1.play(frequency, ms);
	player_2.play(frequency, ms);
	while (blink && (player_1.isPlaying() || player_2.isPlaying())) {
		setLetter(index_letter, blink);
		delay(4);
	}
	delay(tronca);
	setLetter(11, blink); // turn off all LEDs
}

void suonaDoppio(int frequency_1, int frequency_2, float fract) {
	int index_letter_1 = getAmericanIndexOfLetterFromFrequency(frequency_1);
	//int index_letter_2 = getAmericanIndexOfLetterFromFrequency(frequency_2);
	if (isMute) {frequency_1 = 0; frequency_2 = 0;}
	int ms = inputTempo() * fract - tronca;
	player_1.play(frequency_1, ms);
	player_2.play(frequency_2, ms);
	while(player_1.isPlaying() || player_2.isPlaying()) {
		//enableDisplay(DISPLAY_1);
		setLetter(index_letter_1);
		delay(4);
		//disableDisplay(DISPLAY_1);
		//delay(1);
		//enableDisplay(DISPLAY_2);
		//setLetter(index_letter_2);
		//delay(4);
		//disableDisplay(DISPLAY_2);
	}
	delay(tronca);
	setLetter(11); // turn off all LEDs
}

void pausa(float fract) {
	delay(inputTempo() * fract);
	
}


///////// Dont touch - Different Melodies

void introduzione() {
	// Battuta 1
	suonaDoppio(MI5, FA_d4, 1);
	suonaDoppio(MI5, FA_d4, 1);
	pausa(1);
	suonaDoppio(MI5, FA_d4, 1);
	// Battuta 2
	pausa(1);
	suonaDoppio(DO5, FA_d4, 1);
	suonaDoppio(MI5, FA_d4, 1);
	pausa(1);
	
	// Battuta 3
	suonaDoppio(SOL5, SOL4, 1);
	pausa(3);
	// Battuta 4
	suonaDoppio(SOL4, SOL3, 1);
	pausa(3);
}

void OVERWORLDFoglio1Riga2() {
	// Battita 1
	suonaDoppio(DO5, SOL3, 1);
	pausa(2);
	suonaDoppio(SOL4, MI3, 1);
	// Battuta 2
	pausa(2);
	suonaDoppio(MI4, DO3, 1);
	pausa(1);
	// Battuta 3
	pausa(1);
	suonaDoppio(LA4, FA3, 1);
	pausa(1);
	suonaDoppio(SI4, SOL3, 1);
	// Battuta 4
	pausa(1);
	suonaDoppio(LA_d4, FA_d3, 1);
	suonaDoppio(LA4, FA3, 1);
	pausa(1);
}

void OVERWORLDFoglio1Riga3() {
	// Battuta 1
	suonaDoppio(SOL4, MI3, TERZINA);
	suonaDoppio(MI5, DO4, TERZINA);
	suonaDoppio(SOL5, MI4, TERZINA);
	// Battuta 2
	suonaDoppio(LA5, FA4, 1);
	pausa(1);
	suonaDoppio(FA5, RE4, 1);
	suonaDoppio(SOL5, MI4, 1);
	// Battuta 3
	pausa(1);
	suonaDoppio(MI5, DO4, 1);
	pausa(1);
	suonaDoppio(DO5, LA3, 1);
	// Battuta 4
	suonaDoppio(RE5, SI3, 1);
	suonaDoppio(SI4, SOL4, 1);
	pausa(2);
}

void OVERWORLDFoglio2Riga1Battuta1() {
	suona(DO3, 1);
	pausa(1);
	suonaDoppio(SOL5, MI5, 1);
	suonaDoppio(FA_d5, RE_d5, 1);
}

void OVERWORLDFoglio2Riga1() {
	// Battuta 1
	OVERWORLDFoglio2Riga1Battuta1();
	// Battuta 2
	suonaDoppio(FA5, RE5, 1);
	suonaDoppio(RE_d5, SI4, 1);
	suona(DO4, 1);
	suonaDoppio(MI5, DO5, 1);
	// Battuta 3
	suona(FA3, 1);
	suonaDoppio(SOL_d4, MI4, 1);
	suonaDoppio(LA4, FA4, 1);
	suonaDoppio(DO5, SOL4, 1);
	// Battuta 4
	suona(DO4, 1);
	suonaDoppio(LA4, DO4, 1);
	suonaDoppio(DO5, MI4, 1);
	suonaDoppio(RE5, FA4, 1);
}

void OVERWORLDFoglio2Riga2() {
	// Battuta 1
	OVERWORLDFoglio2Riga1Battuta1();
	// Battuta 2
	suonaDoppio(FA5, RE5, 1);
	suonaDoppio(RE_d5, SI4, 1);
	suona(SOL3, 1);
	suonaDoppio(MI5, DO4, 1);
	// Battuta 3
	pausa(1);
	suonaDoppio(DO6, SOL5, 1);
	pausa(1);
	suonaDoppio(DO6, SOL5, 1);
	// Battuta 4
	suonaDoppio(DO6, SOL5, 1);
	pausa(1);
	suona(SOL3, 1);
	pausa(1);
}

void OVERWORLDFoglio2Riga4() {
	// Battuta 1
	suona(DO3, 1);
	pausa(1);
	suonaDoppio(RE_d5, SOL_d3, 1);
	pausa(1);
	// Battuta 2
	pausa(1);
	suonaDoppio(RE5, LA_d3, 1);
	pausa(2);
	// Battuta 3
	suonaDoppio(DO5, DO4, 1);
	pausa(2);
	suona(SOL3, 1);
	// Battuta 4
	suona(SOL3, 1);
	pausa(1);
	suona(DO3, 1);
	pausa(1);
}

void OVERWORLDFoglio4Riga1Battuta1() {
	suonaDoppio(DO5, SOL_d4, 1);
	suonaDoppio(DO5, SOL_d4, 1);
	pausa(1);
	suonaDoppio(DO5, SOL_d4, 1);
}

void OVERWORLDFoglio4Riga1() {
	// Battuta 1
	OVERWORLDFoglio4Riga1Battuta1();
	// Battuta 2
	pausa(1);
	suonaDoppio(DO5, SOL_d4, 1);
	suonaDoppio(RE5, LA_d4, 1);
	pausa(1);
	// Battuta 3
	suonaDoppio(MI5, SOL4, 1);
	suonaDoppio(DO5, MI4, 1);
	pausa(1);
	suonaDoppio(LA4, MI4, 1);
	// Battuta 4
	suonaDoppio(SOL4, DO4, 1);
	pausa(1);
	suona(SOL3, 1); // Non mettere SOL2 perche la frequenza bassa manda a troie tutta la classe Tone
	pausa(1);  
}

void OVERWORLDFoglio4Riga2() {
	// Battuta 1
	OVERWORLDFoglio4Riga1Battuta1();
	// Battuta 2
	pausa(1);
	suonaDoppio(DO5, SOL_d4, 1);
	suonaDoppio(RE5, LA_d4, 1);
	suonaDoppio(MI5, SOL4, 1);
	// Battuta 3
	suona(SOL3, 1);
	pausa(2);
	suona(DO3, 1);
	// Battuta 4
	pausa(2);
	suona(SOL3, 1); // Non mettere SOL2, la frequenza e troppo bassa e manda a puttane tutta la classe Tone
	pausa(1);
}

void OVERWORLDFoglio6Riga1() {
	// Battuta 1
	suonaDoppio(MI5, DO5, 1);
	suonaDoppio(DO5, LA4, 1);
	pausa(1);
	suonaDoppio(SOL4, MI4, 1);
	// Battuta 2
	suona(SOL3, 1);
	pausa(1);
	suonaDoppio(SOL_d4, MI4, 1);
	pausa(1);
	// Battuta 3
	suonaDoppio(LA4, FA4, 1);
	suonaDoppio(FA5, DO5, 1);
	suona(FA3, 1);
	suonaDoppio(FA5, DO5, 1);
	// Battuta 4
	suonaDoppio(LA4, FA4, 1);
	suona(DO4, 1);
	suona(FA3, 1);
	pausa(1);
}

void OVERWORLDFoglio6Riga2() {
	// Battuta 1
	suonaDoppio(SI4, SOL4, TERZINA);
	suonaDoppio(LA5, FA5, TERZINA);
	suonaDoppio(LA5, FA5, TERZINA);
	// Battuta 2
	suonaDoppio(LA5, FA5, TERZINA);
	suonaDoppio(SOL5, MI5, TERZINA);
	suonaDoppio(FA5, RE5, TERZINA);
	// Battuta 3
	suonaDoppio(MI5, DO5, 1);
	suonaDoppio(DO5, LA4, 1);
	suona(SOL3, 1);
	suonaDoppio(LA4, FA4, 1);
	// Battuta 4
	suonaDoppio(SOL4, MI4, 1);
	suona(DO5, 1);
	suona(SOL4, 1);
	pausa(1);
}

void OVERWORLDFoglio6Riga4() {
	// Battuta 1
	suonaDoppio(SI4, SOL4, 1);
	suonaDoppio(FA5, RE5, 1);
	pausa(1);
	suonaDoppio(FA5, RE5, 1);
	// Battuta 2
	suonaDoppio(FA5, RE5, TERZINA);
	suonaDoppio(MI5, DO5, TERZINA);
	suonaDoppio(RE5, SI4, TERZINA);
	// Battuta 3
	suonaDoppio(DO5, SOL4, 1);
	suona(MI4, 1);
	suona(SOL3, 1);
	suona(MI4, 1);
	// Battuta 4
	suona(DO4, 1);
	pausa(3);
}

void OVERWORLDFoglio1() {
	// Riga 2
	OVERWORLDFoglio1Riga2();
	// Riga 3
	OVERWORLDFoglio1Riga3();
	// Riga 4
	OVERWORLDFoglio1Riga2();
	// Riga 5
	OVERWORLDFoglio1Riga3();
}

void OVERWORLDFoglio2() {
	// Riga 1
	OVERWORLDFoglio2Riga1();
	// Riga 2
	OVERWORLDFoglio2Riga2();
	// Riga 3
	OVERWORLDFoglio2Riga1();
	// Riga 4
	OVERWORLDFoglio2Riga4();
}

void OVERWORLDFoglio4() {
	// Riga 1
	OVERWORLDFoglio4Riga1();
	// Riga 2 
	OVERWORLDFoglio4Riga2();
	// Riga 3
	OVERWORLDFoglio4Riga1();
	// Riga 4
	introduzione(); 
}

void OVERWORLDFoglio6() {
	// Riga 1
	OVERWORLDFoglio6Riga1();
	// Riga 2
	OVERWORLDFoglio6Riga2();
	// Riga 3
	OVERWORLDFoglio6Riga1();
	// Riga 4
	OVERWORLDFoglio6Riga4();
}

/* UNDERWORLD */

void UNDERWORLDFoglio1Riga1Battuta1() {
	suonaCoppia(DO4, 1);
	suonaCoppia(DO5, 1);
	suonaCoppia(LA3, 1);
	suonaCoppia(LA4, 1);
}

void UNDERWORLDFoglio1Riga1Battuta2() {
	suonaCoppia(LA_d3, 1);
	suonaCoppia(LA_d4, 1);
	pausa(2);
}

void UNDERWORLDFoglio1Riga1() {
	// Battuta 1
	UNDERWORLDFoglio1Riga1Battuta1();
	// Battuta 2
	UNDERWORLDFoglio1Riga1Battuta2();
	// Battuta 3
	pausa(4);
	// Battuta 4
	UNDERWORLDFoglio1Riga1Battuta1();
	// Battuta 5
	UNDERWORLDFoglio1Riga1Battuta2();
	// Battuta 6
	pausa(4);
}

void UNDERWORLDFoglio1Riga2Battuta1() {
	suonaCoppia(FA3, 1);
	suonaCoppia(FA4, 1);
	suonaCoppia(RE3, 1);
	suonaCoppia(RE4, 1);
}

void UNDERWORLDFoglio1Riga2Battuta2() {
	suonaCoppia(RE_d3, 1);
	suonaCoppia(RE_d4, 1);
	pausa(2);
}

void UNDERWORLDFoglio1Riga2() {
	// Battuta 1
	UNDERWORLDFoglio1Riga2Battuta1();
	// Battuta 2
	UNDERWORLDFoglio1Riga2Battuta2();
	// Battuta 3
	pausa(4);
	// Battuta 4
	UNDERWORLDFoglio1Riga2Battuta1();
	// Battuta 5
	UNDERWORLDFoglio1Riga2Battuta2();
	// Battuta 6
	pausa(2);
	suonaCoppia(RE_d4, TERZINA_2);
	suonaCoppia(RE4, TERZINA_2);
	suonaCoppia(DO_d4, TERZINA_2);
}

void UNDERWORLDFoglio1Riga3() {
	// Battuta 1
	suonaCoppia(DO4, 2);
	suonaCoppia(RE_d4, 2);
	// Battuta 2
	suonaCoppia(RE4, 2);
	suonaCoppia(SOL_d3, 2);
	// Battuta 3
	suonaCoppia(SOL3, 2);
	suonaCoppia(DO_d4, 2);
}

void UNDERWORLDFoglio1Riga4() {
	// Battuta 1
	suonaCoppia(DO4, TERZINA_2);
	suonaCoppia(FA_d4, TERZINA_2);
	suonaCoppia(FA4, TERZINA_2);
	suonaCoppia(MI4, TERZINA_2);
	suonaCoppia(LA_d4, TERZINA_2);
	suonaCoppia(LA4, TERZINA_2);
	// Battuta 2
	suonaCoppia(SOL_d4, TERZINA);
	suonaCoppia(RE_d4, TERZINA);
	suonaCoppia(SI3, TERZINA);
	// Battuta 3
	suonaCoppia(LA_d3, TERZINA);
	suonaCoppia(LA3, TERZINA);
	suonaCoppia(SOL_d3, TERZINA);
}

void UNDERWORLD() {
	// Riga 1
	UNDERWORLDFoglio1Riga1();
	// Riga 2
	UNDERWORLDFoglio1Riga2();
	// Riga 3
	UNDERWORLDFoglio1Riga3();
	// Riga 4
	UNDERWORLDFoglio1Riga4();
	// Riga 4
	pausa(12);
}

void COIN(int n_coin) {
	for(int i=0; i<n_coin; i++) {
		suonaCoppia(SI5, 0.5);  
	}  
	setLetter(10);
	suonaCoppia(MI6, 1.5);
}

void STARMAN() {
	suonaDoppio(DO5, FA4, 1);
	suonaDoppio(DO5, FA4, 1);
	suonaDoppio(DO5, FA4, 1);
	suona(RE4, (float) 1/2);
	suonaDoppio(DO5, FA4, (float) 1/2);
	pausa((float) 1/2);
	suonaDoppio(DO5, FA4, 1);
	suona(RE4, (float) 1/2);
	suonaDoppio(DO5, FA4, (float) 1/2);
	suona(RE4, (float) 1/2);
	suonaDoppio(DO5, FA4, 1);
	suonaDoppio(SI4, MI4, 1);
	suonaDoppio(SI4, MI4, 1);
	suonaDoppio(SI4, MI4, 1);
	suona(DO4, (float) 1/2);
	suonaDoppio(SI4, MI4, (float) 1/2);
	pausa((float) 1/2);
	suonaDoppio(SI4, MI4, 1);
	suona(DO4, (float) 1/2);
	suonaDoppio(SI4, MI4, (float) 1/2);
	suona(DO4, (float) 1/2);
	suonaDoppio(SI4, MI4, 1);
}

void FLAGPOLEFANFARE() {
	int pretronca = tronca;
	tronca = 0;
	float freqPartenza = SOL3;
	float freqFinale = FA_d5;
	float tempo = 0.0851;
	float passoFreq = (freqFinale - freqPartenza) / 47;
	float freq = freqPartenza;
	while(freq<=freqFinale) {
		suona(freq, tempo);
		freq += passoFreq;
	}
	tronca = pretronca;
	suonaCoppia(SOL5, 0.25);
	pausa(5.75);
	suona(SOL3, TERZINA_2);
	suonaDoppio(DO4, MI3, TERZINA_2);
	suonaDoppio(MI4, SOL3, TERZINA_2);
	suonaDoppio(SOL4, DO3, TERZINA_2);
	suonaDoppio(DO5, MI3, TERZINA_2);
	suonaDoppio(MI5, SOL3, TERZINA_2);
	suonaDoppio(SOL5, MI3, 2);
	suonaDoppio(MI5, DO3, 2);
	suona(SOL_d3, TERZINA_2);
	suonaDoppio(DO4, RE_d3, TERZINA_2);
	suonaDoppio(RE_d4, SOL_d3, TERZINA_2);
	suonaDoppio(SOL_d4, DO3, TERZINA_2);
	suonaDoppio(DO5, MI3, TERZINA_2);
	suonaDoppio(RE_d5, SOL_d3, TERZINA_2);
	suonaDoppio(SOL_d5, RE_d3, 2);
	suonaDoppio(RE_d5, DO3, 2);
	
	suona(LA_d3, TERZINA_2);
	suonaDoppio(RE4, LA_d3, TERZINA_2);
	suonaDoppio(FA4, FA3, TERZINA_2);
	suonaDoppio(LA_d4, RE3, TERZINA_2);
	suonaDoppio(RE5, FA3, TERZINA_2);
	suonaDoppio(FA5, LA_d3, TERZINA_2);
	suonaDoppio(LA_d5, RE4, 2);
	suonaDoppio(LA_d5, LA_d3, TERZINA_2);
	suonaDoppio(LA_d5, LA_d3, TERZINA_2);
	suonaDoppio(LA_d5, LA_d3, TERZINA_2);
	suonaDoppio(DO6, DO4, 4);
}

void POWERUP() {
	suonaCoppia(SOL4, 0.2);
	suonaCoppia(SI4, 0.2);
	suonaCoppia(RE5, 0.2);
	suonaCoppia(SOL5, 0.2);
	suonaCoppia(SI5, 0.2);
	suonaCoppia(SOL_d4, 0.2);
	suonaCoppia(DO5, 0.2);
	suonaCoppia(RE_d5, 0.2);
	suonaCoppia(SOL_d5, 0.2);
	suonaCoppia(DO6, 0.2);
	suonaCoppia(LA_d4, 0.2);
	suonaCoppia(RE5, 0.2);
	suonaCoppia(FA5, 0.2);
	suonaCoppia(LA_d5, 0.2);
	suonaCoppia(RE6, 0.2);
	pausa(1);
}

void ITEMBLOCK() {
	suonaDoppio(SOL4, SOL3, 0.5);
	suonaDoppio(SOL_d4, SOL_d3, 0.5);
	suonaDoppio(LA4, LA3, 0.5);
	suonaDoppio(LA_d4, LA_d3, 0.5);
	suonaDoppio(SI4, SI3, 0.5);
	pausa(1.0);
}

void DAMAGEWARP() {
	for(int i=0; i<3; i++) {
		suonaCoppia(LA4, TERZINA_3);
		suonaCoppia(FA4, TERZINA_3);
		suonaCoppia(LA3, TERZINA_3);
		suonaCoppia(DO5, TERZINA_3);
		pausa(1);
	}
	pausa(2);
}

void ONEUP() {
	suonaCoppia(MI5, 1);
	suonaCoppia(SOL5, 1);
	suonaCoppia(MI6, 1);
	suonaCoppia(DO6, 1);
	suonaCoppia(RE6, 1);
	suonaCoppia(SOL6, 1);
	pausa(2);
}

void RESCUEFANFARE() {
	int pretronca = tronca;
	tronca = 0;
	for(int i=0; i<2; i++) {
		suonaDoppio(DO5, MI3, TERZINA_2);
		suonaDoppio(SOL4, MI3, TERZINA_2);
		suonaDoppio(MI4, MI3, TERZINA_2);
	}
	tronca = pretronca;
	suonaDoppio(DO5, MI3, 1);
	suonaDoppio(DO5, MI3, 0.5);
	suonaDoppio(DO5, MI3, 0.5);
	for(int i=0; i<3; i++) {
		suonaDoppio(DO5, MI3, TERZINA_2);
	}
	
	tronca = 0;
	for(int i=0; i<2; i++) {
		suonaDoppio(DO_d5, FA3, TERZINA_2);
		suonaDoppio(SOL_d4, FA3, TERZINA_2);
		suonaDoppio(FA4, FA3, TERZINA_2);
	}
	tronca = pretronca;
	suonaDoppio(DO_d5, FA3, 1);
	suonaDoppio(DO_d5, FA3, 0.5);
	suonaDoppio(DO_d5, FA3, 0.5);
	for(int i=0; i<3; i++) {
		suonaDoppio(DO_d5, FA3, TERZINA_2);
	}
	
	tronca = 0;
	for(int i=0; i<2; i++) {
		suonaDoppio(RE_d5, SOL4, TERZINA_2);
		suonaDoppio(LA_d4, RE_d4, TERZINA_2);
		suonaDoppio(SOL4, LA_d3, TERZINA_2);
	}
	tronca = pretronca;
	suonaDoppio(RE_d5, SOL4, 1);
	suonaDoppio(RE_d5, SOL4, 0.5);
	suonaDoppio(RE_d5, SOL4, 0.5);
	for(int i=0; i<3; i++) {
		suonaDoppio(FA5, LA4, TERZINA_2);
	}
	suonaDoppio(SOL5, SI4, 4);
}

void DEATH() {
	suonaCoppia(DO5, (float) 1/4);
	suonaCoppia(DO_d5, (float) 1/4);
	suonaCoppia(RE5, 0.5);
	pausa(3);
	suonaDoppio(SI4, SOL4, 1);
	suonaDoppio(FA5, RE5, 1);
	pausa(1);
	suonaDoppio(FA5, RE5, 1);
	suonaDoppio(FA5, RE5, TERZINA);
	suonaDoppio(MI5, DO5, TERZINA);
	suonaDoppio(RE5, SI4, TERZINA);
	suonaDoppio(DO5, SOL4, 1);
	suona(MI4, 1);
	suona(SOL3, 1);
	suona(MI4, 1);
	suonaDoppio(DO4, DO3, 1);
	pausa(3);
}

void GAMEOVER() {
	suonaDoppio(DO5, MI4, 1);
	pausa(2);
	suonaDoppio(SOL4, DO4, 1);
	pausa(2);
	suonaDoppio(MI4, SOL3, 2);
	
	suonaDoppio(LA4, FA4, TERZINA);
	suonaDoppio(SI4, FA4, TERZINA);
	suonaDoppio(LA4, FA4, TERZINA);
	
	suonaDoppio(SOL_d4, FA4, 2);
	suonaDoppio(LA_d4, FA4, 2);
	
	suonaDoppio(SOL_d4, FA4, 2);
	suonaDoppio(SOL4, MI4, 1);
	suonaDoppio(SOL4, RE4, 1);
	
	suonaDoppio(SOL4, MI4, 8);
}

void CASTLEBattuta1() {
	suonaDoppio(RE5, LA_d4, SEMICROMA);
	suona(SOL4, SEMICROMA);
	suonaDoppio(DO_d4, LA4, SEMICROMA);
	suona(SOL4, SEMICROMA);
	suonaDoppio(DO4, SOL_d4, SEMICROMA);
	suona(SOL4, SEMICROMA);
	suonaDoppio(DO_d4, LA4, SEMICROMA);
	suona(SOL4, SEMICROMA);
	
	suonaDoppio(RE5, LA_d4, SEMICROMA);
	suona(SOL4, SEMICROMA);
	suonaDoppio(RE_d4, SI4, SEMICROMA);
	suona(SOL4, SEMICROMA);
	suonaDoppio(RE4, LA_d4, SEMICROMA);
	suona(SOL4, SEMICROMA);
	suonaDoppio(DO_d4, LA4, SEMICROMA);
	suona(SOL4, SEMICROMA);
}

void CASTLEBattuta3() {
	suonaDoppio(DO_d5, LA4, SEMICROMA);
	suona(FA_d4, SEMICROMA);
	suonaDoppio(DO4, SOL_d4, SEMICROMA);
	suona(FA_d4, SEMICROMA);
	suonaDoppio(DO_d4, LA4, SEMICROMA);
	suona(FA_d4, SEMICROMA);
	suonaDoppio(RE4, LA_d4, SEMICROMA);
	suona(FA_d4, SEMICROMA);
	
	suonaDoppio(DO_d5, LA4, SEMICROMA);
	suona(FA_d4, SEMICROMA);
	suonaDoppio(RE4, LA_d4, SEMICROMA);
	suona(FA_d4, SEMICROMA);
	suonaDoppio(DO_d4, LA_d4, SEMICROMA);
	suona(FA_d4, SEMICROMA);
	suonaDoppio(DO4, SOL_d4, SEMICROMA);
	suona(FA_d4, SEMICROMA); 
}

void CASTLEBattuta5() {
	suonaDoppio(FA5, DO_d5, SEMICROMA);
	suona(LA_d4, SEMICROMA);
	suonaDoppio(FA_d5, RE5, SEMICROMA);
	suona(LA_d4, SEMICROMA);
	suonaDoppio(FA5, DO_d5, SEMICROMA);
	suona(LA_d4, SEMICROMA);
	suonaDoppio(MI5, DO4, SEMICROMA);
	suona(LA_d4, SEMICROMA);
	
	suonaDoppio(FA5, DO_d5, SEMICROMA);
	suona(LA_d4, SEMICROMA);
	suonaDoppio(MI5, DO5, SEMICROMA);
	suona(LA_d4, SEMICROMA);
	suonaDoppio(RE_d5, SI4, SEMICROMA);
	suona(LA_d4, SEMICROMA);
	suonaDoppio(MI5, DO5, SEMICROMA);
	suona(LA_d4, SEMICROMA);
}

void CASTLE() {
	int pretronca = tronca;
	tronca = 0; 
	for(int i=0; i<2; i++) {
		CASTLEBattuta1();
		CASTLEBattuta1();
		CASTLEBattuta3();
		CASTLEBattuta3();
		CASTLEBattuta5();
		CASTLEBattuta5();
	}
	tronca = pretronca;  
}

void UNDERWATER() {
	int pretronca = tronca;
	tronca = 0;
	
	suonaDoppio(RE4, RE4, 1);
	suonaDoppio(MI4, DO_d4, 1);
	suonaDoppio(FA_d4, DO4, 1);
	
	suonaDoppio(SOL4, SI3, 1);
	suonaDoppio(LA4, DO4, 1);
	suonaDoppio(LA_d4, DO_d4, 1);
	
	suonaDoppio(SI4, RE4, 0.5);
	suonaDoppio(SI4, RE4, 0.5);
	suonaDoppio(SI4, RE4, 1);
	suonaDoppio(SI4, SOL3, 1);
	suonaDoppio(SI4, SOL3, 2);
	suona(SOL4, 1);
	
	// Riga 2
	suonaDoppio(MI5, DO3, 1);
	suonaDoppio(MI5, SOL3, 1);
	suonaDoppio(MI5, DO4, 1);
	
	suonaDoppio(RE_d5, SI2, 1);
	suonaDoppio(RE_d5, SOL3, 1);
	suonaDoppio(RE_d5, SI3, 1);
	
	suonaDoppio(MI5, DO3, 1);
	suonaDoppio(MI5, SOL3, 1);
	suonaDoppio(MI5, DO4, 1);
	
	suona(MI4, 0.5);
	suonaDoppio(SOL4, MI4, 0.5);
	suonaDoppio(LA4, SOL3, 0.5);
	suonaDoppio(SI4, SOL3, 0.5);
	suonaDoppio(DO5, DO4, 0.5);
	suonaDoppio(RE5, DO4, 0.5);
	
	// Riga 3
	suonaDoppio(MI5, DO3, 1);
	suonaDoppio(MI5, SOL3, 1);
	suonaDoppio(MI5, DO4, 1);
	
	suonaDoppio(RE_d5, SI2, 1);
	suonaDoppio(RE_d5, SOL3, 1);
	suonaDoppio(FA5, SI3, 1);
	
	suonaDoppio(MI5, DO3, 1);
	suonaDoppio(MI5, SOL3, 1);
	suonaDoppio(MI5, DO4, 1);
	
	suona(MI3, 1);
	suona(SOL3, 1);
	suona(DO4, 0.5);
	suona(SOL4, 0.5);
	
	// Riga 4
	suonaDoppio(RE5, RE3, 1);
	suonaDoppio(RE5, SOL3, 1);
	suonaDoppio(RE5, SI3, 1);
	
	suonaDoppio(DO_d5, DO_d3, 1);
	suonaDoppio(DO_d5, FA_d3, 1);
	suonaDoppio(DO_d5, LA_d3, 1);
	
	suonaDoppio(RE5, RE3, 1);
	suonaDoppio(RE5, SOL3, 1);
	suonaDoppio(RE5, SI3, 1);
	
	suona(SI2, 0.5);
	suonaDoppio(SOL4, SI2, 0.5);
	suonaDoppio(LA4, SOL3, 0.5);
	suonaDoppio(SI4, SOL3, 0.5);
	suonaDoppio(DO5, SI3, 0.5);
	suonaDoppio(DO_d5, SI3, 0.5);
	
	// Foglio 2
	
	// Riga 1
	suonaDoppio(RE5, RE3, 1);
	suonaDoppio(RE5, SOL3, 1);
	suonaDoppio(RE5, SI3, 1);
	
	suonaDoppio(SOL4, SI2, 1);
	suonaDoppio(SOL4, SOL3, 1);
	suonaDoppio(FA5, SI3, 1);
	
	suonaDoppio(MI5, DO3, 1);
	suonaDoppio(MI5, SOL3, 1);
	suonaDoppio(MI5, SOL4, 1);
	
	suona(DO3, 1); // Non posso fare SOL2: Arduino esplode
	suona(SOL3, 1);
	suona(DO4, 0.5);
	suonaDoppio(SOL4, DO4, 0.5);
	
	// Riga 2
	suonaDoppio(SOL5, DO3, 1);
	suonaDoppio(SOL5, SOL3, 1);
	suonaDoppio(SOL5, MI4, 1);
	
	suonaDoppio(SOL5, SI2, 1);
	suonaDoppio(SOL5, SOL3, 1);
	suonaDoppio(SOL5, RE4, 1);
	
	suonaDoppio(SOL5, SI2, 1); // CON LA_d2 Esplode!
	suonaDoppio(SOL5, SOL3, 1);
	suonaDoppio(SOL5, DO_d4, 1);
	
	suonaDoppio(SOL5, DO_d3, 1);
	suonaDoppio(LA5, SOL3, 1);
	suona(MI4, 0.5);
	suonaDoppio(SOL5, MI4, 0.5);
	
	// Riga 3
	suonaDoppio(FA5, RE3, 1);
	suonaDoppio(FA5, LA3, 1);
	suonaDoppio(FA5, RE4, 1);
	
	suonaDoppio(FA5, DO_d3, 1);
	suonaDoppio(FA5, LA3, 1);
	suonaDoppio(FA5, DO_d4, 1);
	
	suonaDoppio(FA5, DO3, 1);
	suonaDoppio(FA5, LA3, 1);
	suonaDoppio(FA5, RE4, 1);
	
	suonaDoppio(FA5, SI2, 1);
	suonaDoppio(SOL5, SOL3, 1);
	suona(RE4, 0.5);
	suonaDoppio(FA5, RE4, 0.5);
	
	// Riga 4
	
	suonaDoppio(MI5, DO3, 1); // COn DO2 esplode!
	suonaDoppio(MI5, SOL3, 1);
	suonaDoppio(MI5, MI4, 1);
	
	suonaDoppio(LA4, SOL3, 1); // Con SOL2 esplode!
	suonaDoppio(SI4, SOL3, 1);
	suonaDoppio(FA5, SOL3, 1);
	
	suonaDoppio(MI5, MI3, 0.5);
	suonaDoppio(MI5, MI3, 0.5);
	suonaDoppio(MI5, MI3, 1.5);
	suonaDoppio(SI4, SI2, 0.5);
	
	suonaDoppio(DO5, DO3, 3);
	
	tronca = pretronca;
	
}

void OVERWORLD() {
	introduzione();
	OVERWORLDFoglio1();
	// Foglio 2
	OVERWORLDFoglio2();
	// Foglio 3
	OVERWORLDFoglio2();
	// Foglio 4
	OVERWORLDFoglio4();
	// Foglio 5
	OVERWORLDFoglio1();
	// Foglio 6
	OVERWORLDFoglio6();
	// Foglio 7
	//OVERWORLDFoglio6();
	// Foglio 8
	//OVERWORLDFoglio4();
	// Foglio 9
	//OVERWORLDFoglio6(); 
}

//--------------------------------------------

void playAllMusic(){
	lcd.clear();
	lcd.print(F("PowerUp"));
	POWERUP();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("OneUp"));
	ONEUP();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("OverWorld"));
	OVERWORLD();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("UnderWater"));
	UNDERWATER();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("UnderWorld"));
	UNDERWORLD();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("Castle"));
	CASTLE();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("Item block"));
	ITEMBLOCK();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("FlagPole"));
	FLAGPOLEFANFARE();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("DamageWarp"));
	DAMAGEWARP();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("Coins"));
	COIN(1);
	COIN(1);
	COIN(5);
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("StarMan"));
	STARMAN();
	pausa(5);
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("Death"));
	DEATH();
	if (Serial.available() > 0) {return;}
	
	lcd.clear();
	lcd.print(F("Game Over"));
	GAMEOVER();
}

int NoteStan[3][12] = {{DO3, DO_d3, RE3, RE_d3, MI3, FA_d3, FA3, SOL_d3, SOL3, LA_d3, LA3, SI3},
	{DO4, DO_d4, RE4, RE_d4, MI4, FA_d4, FA4, SOL_d4, SOL4, LA_d4, LA4, SI4},
{DO5, DO_d5, RE5, RE_d5, MI5, FA_d5, FA5, SOL_d5, SOL5, LA_d5, LA4, SI5}};

String StrNoteStan[12] = {"DO", "DO#", "RE", "RE#", "MI", "FA#", "FA", "SOL#", "SOL", "LA#", "LA", "SI"};

// Play Color
void PlayColor(byte LED_color, float fract = 1, bool blink = false) {
	if (isMute) {delay(200);} // Long Blink if NOT Sound
	suonaCoppia(ButtonNote[LED_color], fract, blink);
}

void beep (int speakerPin, float noteFrequency, long noteDuration)
{
	int x;
	if (!isMute) {
		// Convert the frequency to microseconds
		float microsecondsPerWave = 1000000/noteFrequency;
		// Calculate how many milliseconds there are per HIGH/LOW cycles.
		float millisecondsPerCycle = 1000/(microsecondsPerWave * 2);
		// Multiply noteDuration * number or cycles per millisecond
		float loopTime = noteDuration * millisecondsPerCycle;
		// Play the note for the calculated loopTime.
		for (x=0;x<loopTime;x++)
		{
			digitalWrite(speakerPin,HIGH);
			delayMicroseconds(microsecondsPerWave);
			digitalWrite(speakerPin,LOW);
			delayMicroseconds(microsecondsPerWave);
		}
	}
}
