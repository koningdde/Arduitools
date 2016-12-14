#include "GetChar.h"
#include "keyboard.h"

//**************************************************//
//   Type the String to Convert to Morse Code Here  //
//**************************************************//

char stringToMorseCode[] = TEKST;
char space[] = " ";
char output[sizeof(stringToMorseCode) + sizeof(space)];

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output for LED lights.
  pinMode(led12, OUTPUT); 
  pinMode(led6, OUTPUT); 
  Serial.begin(9600);

  sprintf(output,"%s%s",stringToMorseCode,space);
  keyboardInit();

  
}
// Create a loop of the letters/words you want to output in morse code (defined in string at top of code)
void loop()
{ 
  // Loop through the string and get each character one at a time until the end is reached
  for (int i = 0; i < sizeof(output) - 1; i++)
  {
    // Get the character in the current position
  char tmpChar = output[i];
  // Set the case to lower case
  tmpChar = toLowerCase(tmpChar);
  Serial.print(tmpChar);
  // Call the subroutine to get the morse code equivalent for this character
  GetChar(tmpChar);
  }
  
  // At the end of the string long pause before looping and starting again
  LightsOff(1500);  
  keys();    
}


