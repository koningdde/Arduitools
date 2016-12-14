int dotLen = 100;     // length of the morse code 'dot'
int dashLen = dotLen * 3;    // length of the morse code 'dash'
int elemPause = dotLen;  // length of the pause between elements of a character
int Spaces = dotLen * 3;     // length of the spaces between characters
int wordPause = dotLen * 7;  // length of the pause between words

// DOT
void MorseDot()
{
  digitalWrite(led12, HIGH);    // turn the LED on 
  digitalWrite(led6, HIGH); 
  tone(audio8, note, dotLen); // start playing a tone
  delay(dotLen);              // hold in this position
}

// DASH
void MorseDash()
{
  digitalWrite(led12, HIGH);    // turn the LED on 
  digitalWrite(led6, HIGH);
  tone(audio8, note, dashLen);  // start playing a tone
  delay(dashLen);               // hold in this position
}

// Turn Off
void LightsOff(int delayTime)
{
  digitalWrite(led12, LOW);     // turn the LED off   
  digitalWrite(led6, LOW);
  noTone(audio8);             // stop playing a tone
  delay(delayTime);             // hold in this position
}
