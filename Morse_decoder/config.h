//Definieer hier de variable

#define TEKST "!Danny";
#define MORSESPEED 100;
#define LED 13;
#define LED2 3;
#define BUZZER 5;
#define toneFr 1200;
#define mycall "PD1DDK";
#define red 11;
#define green 10;
#define bleu 9;

// Create variable to define the output pins
int led12 = LED;      // blink an led on output 12
int led6 = LED2;        // blink an led on output 6
int audio8 = BUZZER;      // output audio on pin 8
int ledr = red;        // blink an led on output 6
int ledg = green;        // blink an led on output 6
int ledb = bleu;        // blink an led on output 6
int note = toneFr;      // music note/pitch

int dotLen = MORSESPEED;     // length of the morse code 'dot'
int dashLen = dotLen * 3;    // length of the morse code 'dash'
int elemPause = dotLen;  // length of the pause between elements of a character
int Spaces = dotLen * 3;     // length of the spaces between characters
int wordPause = dotLen * 7;  // length of the pause between words

