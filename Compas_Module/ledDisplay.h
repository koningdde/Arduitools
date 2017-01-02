
//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

void registerWrite(int whichPin) {
  // turn off the output so the pins don't light up
  // while you're shifting bits:
  digitalWrite(latchPin, LOW);
  delay(5); //little delay to drop led flickering
  // shift the bits out:
  shiftOut(dataPin, clockPin, MSBFIRST, (whichPin));
  // turn on the output so the LEDs can light up:
  digitalWrite(latchPin, HIGH);
}

//Write heading from compas to led display
void showLedDisplay(int headingDegrees) {

  if (headingDegrees < 0 || headingDegrees > 360) {
    registerWrite(0);
  }
  else if (headingDegrees >= 0 && headingDegrees <= 11)
  {
    registerWrite(1);
  }
  else if (headingDegrees > 349 && headingDegrees <= 360)
  {
    registerWrite(1);
  }
  else if (headingDegrees > 11 && headingDegrees <= 34)
  {
    registerWrite(9);
  }
  else if (headingDegrees > 34 && headingDegrees <= 56)
  {
    registerWrite(8);
  }
  else if (headingDegrees > 56 && headingDegrees <= 79)
  {
    registerWrite(10);
  }
  else if (headingDegrees > 79 && headingDegrees <= 101)
  {
    registerWrite(2);
  }
  else if (headingDegrees > 101 && headingDegrees <= 124)
  {
    registerWrite(6);
  }
  else if (headingDegrees > 124 && headingDegrees <= 146)
  {
    registerWrite(4);
  }
  else if (headingDegrees > 146 && headingDegrees <= 169)
  {
    registerWrite(20);
  }
  else if (headingDegrees > 169 && headingDegrees <= 191)
  {
    registerWrite(16);
  }
  else if (headingDegrees > 191 && headingDegrees <= 214)
  {
    registerWrite(80);
  }
  else if (headingDegrees > 214 && headingDegrees <= 236)
  {
    registerWrite(64);
  }
  else if (headingDegrees > 236 && headingDegrees <= 259)
  {
    registerWrite(192);
  }
  else if (headingDegrees > 259 && headingDegrees <= 281)
  {
    registerWrite(128);
  }
  else if (headingDegrees > 281 && headingDegrees <= 304)
  {
    registerWrite(160);
  }
  else if (headingDegrees > 304 && headingDegrees <= 326)
  {
    registerWrite(32);
  }
  else if (headingDegrees > 326 && headingDegrees <= 349)
  {
    registerWrite(33);
  }
  
}
