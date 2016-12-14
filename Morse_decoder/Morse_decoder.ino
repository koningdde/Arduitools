//**************************************************//
//   Type the String to Convert to Morse Code Here  //
//**************************************************//
char stringToMorseCode[] = "a.,9 ";

// Create variable to define the output pins
int led12 = 13;      // blink an led on output 12
int led6 = 6;        // blink an led on output 6
int audio8 = 8;      // output audio on pin 8
int note = 1200;      // music note/pitch
#include "GetChar.h"

/*
  Set the speed of your morse code
  Adjust the 'dotlen' length to speed up or slow down your morse code
    (all of the other lengths are based on the dotlen)

  Here are the ratios code elements:
    Dash length = Dot length x 3
    Pause between elements = Dot length
      (pause between dots and dashes within the character)
    Pause between characters = Dot length x 3
    Pause between words = Dot length x 7
  
*/


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output for LED lights.
  pinMode(led12, OUTPUT); 
  pinMode(led6, OUTPUT); 
  Serial.begin(9600);
}

// Create a loop of the letters/words you want to output in morse code (defined in string at top of code)
void loop()
{ 
  // Loop through the string and get each character one at a time until the end is reached
  for (int i = 0; i < sizeof(stringToMorseCode) - 1; i++)
  {
    // Get the character in the current position
  char tmpChar = stringToMorseCode[i];
  // Set the case to lower case
  tmpChar = toLowerCase(tmpChar);
  Serial.print(tmpChar);
  // Call the subroutine to get the morse code equivalent for this character
  GetChar(tmpChar);
  }
  
  // At the end of the string long pause before looping and starting again
  LightsOff(1500);      
}

