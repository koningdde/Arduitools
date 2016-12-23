//Define your settings

#define mycall "PD1DDK";          //Your callsign F1
#define CQ "CQ CQ";                //Define number of CQ
#define TEKST2 "Mobile station";  //Template tekst F2
#define TEKST3 "My name is Danny";   //Template tekst F3
#define MORSESPEED 100;           //Initial dot lenght
#define LED 13;                   //Output pin to morse
#define LED2 3;                   //Morse output to led
#define BUZZER 5;                 //Pin to connect internal buzzer
#define toneFr 1200;              //Internal buzzer tone freq
#define red 11;                   //Red pin rgb led
#define green 10;                 //Green pin rgb led
#define bleu 9;                   //Blue pin rgb led
#define decoderpin 8;             //For connection output from de tone decoder

//End off settings

// Create variable to define the output pins
const int led12 = LED;      // blink an led on output 12
const int led6 = LED2;        // blink an led on output 6
const int audio8 = BUZZER;      // output audio on pin 8
const int ledr = red;        // blink an led on output 6
const int ledg = green;        // blink an led on output 6
const int ledb = bleu;        // blink an led on output 6
const int note = toneFr;      // music note/pitch

//Define morse protocol standards
int dotLen = MORSESPEED;     // length of the morse code 'dot'
int dashLen = dotLen * 3;    // length of the morse code 'dash'
int elemPause = dotLen;  // length of the pause between elements of a character
int Spaces = dotLen * 3;     // length of the spaces between characters
int wordPause = dotLen * 7;  // length of the pause between words

