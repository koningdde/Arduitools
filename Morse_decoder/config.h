//Definieer hier de variable

#define TEKST "!Danny";
#define MORSESPEED 20;
#define LED 13;
#define LED2 3;
#define BUZZER 8;
#define toneFr 1200;

// Create variable to define the output pins
int led12 = LED;      // blink an led on output 12
int led6 = LED2;        // blink an led on output 6
int audio8 = BUZZER;      // output audio on pin 8
int note = toneFr;      // music note/pitch

int dotLen = MORSESPEED;     // length of the morse code 'dot'
int dashLen = dotLen * 3;    // length of the morse code 'dash'
int elemPause = dotLen;  // length of the pause between elements of a character
int Spaces = dotLen * 3;     // length of the spaces between characters
int wordPause = dotLen * 7;  // length of the pause between words

