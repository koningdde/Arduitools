//Make your settings only in config.h

#include "Arduino.h"      //Define default
#include "GetChar.h"      //Change char to dih-dah's
#include "PS2Keyboard.h"  //For use of an ps2 keyboard
#include "decode.h"       //Signal input decoder
const int DataPin = 6;    // Keyboard data pin
const int IRQpin =  2;    // Keyboard clock pin
PS2Keyboard keyboard;     //Make keyboard
int i = 0;                //Counter for number off char
int r;                    //Counter for number off

char mycallout[] = mycall;                //Array for callsign
char cqout[] = CQ;                        //Array for callsign
char F2[] = TEKST2;                       //Pre defined tekst F2
char F3[] = TEKST3;                       //Pre defined tekst F3
char space[] = " ";                       //Space to add after char
char k[] = "k";                           //Over
char sk[] = "sk";                         //Close connection
char cl[] = "73";                         //Greetings

char output1[sizeof(cqout) + sizeof(space) + sizeof(mycallout) + sizeof(space) + sizeof(k) + sizeof(space)]; //Make cqcq
char output2[sizeof(F2) + sizeof(space)];                                        //Make array the sise of two other array's
char output3[sizeof(F3) + sizeof(space)];                                        //Make array the sise of two other array's
char input[40];                                                                  //Array for keyboard input

// the setup routine runs once when you press reset:
void setup() {  
  //Start keyboard
  keyboard.begin(DataPin, IRQpin, PS2Keymap_US);               
  // initialize the digital pins.
  pinMode(led12, OUTPUT); 
  pinMode(led6, OUTPUT);
  pinMode(audioPin, INPUT); 
  pinMode(ledr, OUTPUT); 
  pinMode(ledg, OUTPUT); 
  pinMode(ledb, OUTPUT); 
  //Begin serial communication
  Serial.begin(9600);
  //Combine arrays to usefull arrays you can send
  sprintf(output1,"%s%s%s%s%s%s",cqout,space,mycallout,space,k,space);  //Add three array's together
  sprintf(output2,"%s%s",F2,space);            //Add two array's together 
  sprintf(output3,"%s%s",F3,space);            //Add two array's together 
  //Showoff rgb led
  statusled('r'); //change led to red
  delay(350);
  statusled('g'); //change led to green
  delay(350);
  statusled('b'); //change led to blue
  delay(350);
//End of setup  
}

// the loop routine
void loop(){    
    statusled('b');   //Make nice blue light
    decoder();        //listen to signal input
           
    if (keyboard.available()) {  //Tis is an interrupt !
    // read the next key
    char c = keyboard.read();
   
    // check for some of the special keys
    if (c == PS2_ENTER) {
      sendkeys(i);                  //Send the contains off the keyboard array to morse output
      i = 0;
    } else if (c == PS2_TAB) {
      matchSpeed();                 //Make TX speed same as RX speed
    } else if (c == PS2_ESC) {
      changecall();                 //Start loop to change call
    } else if (c == PS2_PAGEDOWN) {
      ;
    } else if (c == PS2_PAGEUP) {
      ;
    } else if (c == PS2_LEFTARROW) {
      ;
    } else if (c == PS2_RIGHTARROW) {
      
    } else if (c == PS2_UPARROW) {
      speedup();                    //Speedup the morse
    } else if (c == PS2_DOWNARROW) {
      speeddown();                  //Speeddown the morse
    } else if (c == PS2_DELETE) {
      i = i - 1;                    //Clear char
    } else if (c == PS2_F1) {
      sprintf(input,"%s",output1);
      sendkeys(sizeof(output1));    //Send CQ
      i = 0;
    } else if (c == PS2_F2) {
      sprintf(input,"%s",output2);
      sendkeys(sizeof(output2));    //Send F2 template
      i = 0;
    } else if (c == PS2_F3) {
      sprintf(input,"%s",output3);
      sendkeys(sizeof(output3));    //Send F3 template
      i = 0;
    } else {
      // no special key, then add char to array
      input[i] = c;
      i = i + 1;
     }
  }
//End of loop       
}

void sendkeys(int a) {

  statusled('r');
  // Loop through the string and get each character one at a time until the end is reached
  for (int b = 0; b <= a -1 ; b++)
  {
    // Get the character in the current position
  char tmpChar = input[b];
  // Set the case to lower case
  tmpChar = toLowerCase(tmpChar);
  Serial.print(tmpChar);
  // Call the subroutine to get the morse code equivalent for this character
  GetChar(tmpChar);
  }
    // At the end of the string long pause before looping and starting again
  Serial.println();
  LightsOff(10); 
}

void speedup(){
  statusled('g');
  dotLen = dotLen - 10;     // length of the morse code 'dot'
  correctspeed();
  Serial.print("Speed is: ");
  Serial.println(dotLen);
  delay(100);
}

void speeddown(){
  statusled('g');
  dotLen = dotLen + 10;     // length of the morse code 'dot'
  correctspeed();
  Serial.print("Speed is: ");
  Serial.println(dotLen);
  delay(100);
}

//Correct al speeds to meet dotLen
void correctspeed(){ 
  dashLen = dotLen * 3;    // length of the morse code 'dash'
  elemPause = dotLen;      // length of the pause between elements of a character
  Spaces = dotLen * 3;     // length of the spaces between characters
  wordPause = dotLen * 7;  // length of the pause between words
}

//Match TX speed to RX speed 
void matchSpeed(){
  statusled('g');   //Make nice blue light
  dotLen = (averageDah/3); //Read avarage dash time from RX and set to TX speed
  correctspeed();
  Serial.print("Speed is: ");
  Serial.println(dotLen);
  delay(100);
}

// change color led
void statusled(char color){
  switch (color) {
    case 'r':
    digitalWrite(ledr, HIGH);
    digitalWrite(ledg, LOW);
    digitalWrite(ledb, LOW);
    break;
    case 'g':
    digitalWrite(ledr, LOW);
    digitalWrite(ledg, HIGH);
    digitalWrite(ledb, LOW);
    break;
    case 'b':
    digitalWrite(ledr, LOW);
    digitalWrite(ledg, LOW);
    digitalWrite(ledb, HIGH);
    break;
  }
}

void changecall(){
  statusled('g');
  boolean hold = true;
  i = 0;
  Serial.println("Change call:");
  while(hold){
    if (keyboard.available()) {
    // read the next key
    char c = keyboard.read();
   
    // check for some of the special keys
      if (c == PS2_ENTER) {
        hold = false;
    } else {
      // no special key, then add char to array
      mycallout[i] = c;
      i = i + 1;
     }
    }
  }
   output1[sizeof(cqout) + sizeof(space) + sizeof(mycallout) + sizeof(space) + sizeof(k) + sizeof(space)];  //Change array to new size
   sprintf(output1,"%s%s%s%s%s%s",cqout,space,mycallout,space,k,space);                                     //Make new CQ
   Serial.print("call is: ");
   Serial.println(mycallout);
}

//morse tone decoder
 void decoder() {
   audio = digitalRead(audioPin); // What is the tone decoder doing?

   if (!audio) keyIsDown();       // LOW, or 0, means tone is being decoded
   if (audio) keyIsUp();          // HIGH, or 1, means no tone is there
 }

 void keyIsDown() {
   // The decoder is detecting our tone
   // The LEDs on the decoder and Arduino will blink on in unison
   digitalWrite(13,1);            // turn on Arduino's LED
     
   if (startUpTime>0){
     // We only need to do once, when the key first goes down
     startUpTime=0;    // clear the 'Key Up' timer
     }
   // If we haven't already started our timer, do it now
   if (startDownTime == 0){
       startDownTime = millis();  // get Arduino's current clock time
      }

     characterDone=false; // we're still building a character
     ditOrDah=false;      // the key is still down we're not done with the tone
     delay(myBounce);     // Take a short breath here
     
   if (myNum == 0) {      // myNum will equal zero at the beginning of a character
      myNum = 1;          // This is our start bit  - it only does this once per letter
      }
 }
 
  void keyIsUp() {
   // The decoder does not detect our tone
   // The LEDs on the decoder and Arduino will blink off in unison 
   digitalWrite(13,0);    // turn off Arduino's LED
   
   // If we haven't already started our timer, do it now
   if (startUpTime == 0){startUpTime = millis();}
   
   // Find out how long we've gone with no tone
   // If it is twice as long as a dah print a space
   upTime = millis() - startUpTime;
   if (upTime<10)return;
   if (upTime > (averageDah*2)) {    
      printSpace();
   }
   
   // Only do this once after the key goes up
   if (startDownTime > 0){
     downTime = millis() - startDownTime;  // how long was the tone on?
     startDownTime=0;      // clear the 'Key Down' timer
   }
 
   if (!ditOrDah) {   
     // We don't know if it was a dit or a dah yet
      shiftBits();    // let's go find out! And do our Magic with the bits
    }

    // If we are still building a character ...
    if (!characterDone) {
       // Are we done yet?
       if (upTime > dit) { 
         // BINGO! we're done with this one  
         printCharacter();       // Go figure out what character it was and print it       
         characterDone=true;     // We got him, we're done here
         myNum=0;                // This sets us up for getting the next start bit
         }
         downTime=0;               // Reset our keyDown counter
       }
   }
   
   
void shiftBits() {
  // we know we've got a dit or a dah, let's find out which
  // then we will shift the bits in myNum and then add 1 or not add 1
  
  if (downTime < dit / 3) return;  // ignore my keybounce
  
  myNum = myNum << 1;   // shift bits left
  ditOrDah = true;        // we will know which one in two lines 
  
  
  // If it is a dit we add 1. If it is a dah we do nothing!
  if (downTime < dit) {
     myNum++;           // add one because it is a dit
     } else {
  
    // The next three lines handle the automatic speed adjustment:
    averageDah = (downTime+averageDah) / 2;  // running average of dahs
    dit = averageDah / 3;                    // normal dit would be this
    dit = dit * 2;    // double it to get the threshold between dits and dahs
     }
  }


void printCharacter() {           
  justDid = false;         // OK to print a space again after this
  
  // Punctuation marks will make a BIG myNum
  if (myNum > 63) {  
    printPunctuation();  // The value we parsed is bigger than our character array
                         // It is probably a punctuation mark so go figure it out.
    return;              // Go back to the main loop(), we're done here.
  }
  lcdGuy = mySet[myNum]; // Find the letter in the character set
  sendToLCD();           // Go figure out where to put in on the display
}

void printSpace() {
  if (justDid) return;  // only one space, no matter how long the gap
  justDid = true;       // so we don't do this twice
  
  // We keep track of the average gap between words and bump it up 20 milliseconds
  // do avoid false spaces within the word
  averageWordGap = ((averageWordGap + upTime) / 2) + 20;

  lastWordCount=0;      // start counting length of word again
  currentLine[letterCount]=' ';  // and a space to the variable that stores the current line
  lastSpace=letterCount;         // keep track of this, our last, space
  
  // Now we need to clear all the characters out of our last word array
  for (int i=0; i<20; i++) {
    lastWord[i]=' ';
   }
   
  lcdGuy=' ';            // this is going to go to the LCD 
  
  // We don't need to print the space if we are at the very end of the line
  if (letterCount < 20) { 
    sendToLCD();         // go figure out where to put it on the display
 }
}

void printPunctuation() {
  // Punctuation marks are made up of more dits and dahs than
  // letters and numbers. Rather than extend the character array
  // out to reach these higher numbers we will simply check for
  // them here. This funtion only gets called when myNum is greater than 63
  
  // Thanks to Jack Purdum for the changes in this function
  // The original uses if then statements and only had 3 punctuation
  // marks. Then as I was copying code off of web sites I added
  // characters we don't normally see on the air and the list got
  // a little long. Using 'switch' to handle them is much better.


  switch (myNum) {
    case 71:
      lcdGuy = ':';
      break;
    case 76:
      lcdGuy = ',';
      break;
    case 84:
      lcdGuy = '!';
      break;
    case 94:
      lcdGuy = '-';
      break;
    case 97:
      lcdGuy = 39;    // Apostrophe
      break;
    case 101:
      lcdGuy = '@';
      break;
    case 106:
      lcdGuy = '.';
      break;
    case 115:
      lcdGuy = '?';
      break;
    case 246:
      lcdGuy = '$';
      break;
    case 122:
      lcdGuy = 's';
      sendToLCD();
      lcdGuy = 'k';
      break;
    default:
      lcdGuy = '#';    // Should not get here
      break;
  }
  sendToLCD();    // go figure out where to put it on the display
}

void sendToLCD(){
  // Do this only if the character is a 'space'
  if (lcdGuy > ' '){
   lastWord[lastWordCount] = lcdGuy; // store the space at the end of the array
   if (lastWordCount < lineEnd - 1) {
     lastWordCount++;   // only bump up the counter if we haven't reached the end of the line
   }
  }
  currentLine[letterCount] = lcdGuy; // now store the character in our current line array
 
  letterCount++;                     // we're counting the number of characters on the line

  // If we have reached the end of the line we will go do some chores
  if (letterCount == lineEnd) {
    newLine();  // check for word wrap and get ready for the next line
    return;     // so we don't need to do anything more here
  }
  
  Serial.print(lcdGuy); // print our character at the current cursor location
 
}

//////////////////////////////////////////////////////////////////////////////////////////
// The following functions handle word wrapping and line scrolling for a 4 line display //
//////////////////////////////////////////////////////////////////////////////////////////

void newLine() {
  // sendToLCD() will call this routine when we reach the end of the line
  if (lastSpace == 0){
    // We just printed an entire line without any spaces in it.
    // We cannot word wrap this one so this character has to go at 
    // the beginning of the next line.
    
    // First we need to clear all the characters out of our last word array
    for (int i=0; i<20; i++) {
      lastWord[i]=' ';
     }
     
     lastWord[0]=lcdGuy;  // store this character in the first position of our next word
     lastWordCount=1;     // set the length to 1
   }
  
  truncateOverFlow();    // Trim off the first part of a word that needs to go on the next line
  linePrep();            // Store the current line so we can move it up later
  reprintOverFlow();     // Print the truncated text and space padding on the next line 
  }

void truncateOverFlow(){
  // Our word is running off the end of the line so we will
  // chop it off at the last space and put it at the beginning of the next line
  
  if (lastSpace==0) {return;}  // Don't do this if there was no space in the last line
  
  // Move the cursor to the place where the last space was printed on the current line
  //lcd.setCursor(lastSpace,LCDline);
  
  letterCount = lastSpace;    // Change the letter count to this new shorter length
  
  // Print 'spaces' over the top of all the letters we don't want here any more
  for (int i = lastSpace; i < 20; i++) {
     Serial.print(' ');         // This space goes on the display
     currentLine[i] = ' ';   // This space goes in our array
  }
}

void linePrep(){
     LCDline++;           // This is our line number, we make it one higher
     
     // What we do next depends on which line we are moving to
     // The first three cases are pretty simple because we working on a cleared
     // screen. When we get to the bottom, though, we need to do more.
     switch (LCDline) {
     case 1:
       // We just finished line 0
       // don't need to do anything because this for the top line
       // it is going to be thrown out when we scroll anyway.
       break;
     case 2:
       // We just finished line 1
       // We are going to move the contents of our current line into the line1 array
       for (int j=0; j<20; j++){
         line1[j] = currentLine[j];
       }
        break;
     case 3:
       // We just finished line 2
       // We are going to move the contents of our current line into the line2 holding bin
       for (int j=0; j<20; j++){
         line2[j] = currentLine[j];
       }
       break;
     case 4:
       // We just finished line 3
       // We are going to move the contents of our current line into the line3 holding bin
       for (int j=0; j<20; j++){
         line3[j] = currentLine[j];
       }
       //This is our bottom line so we will keep coming back here
       LCDline = 3;  //repeat this line over and over now. There is no such thing as line 4
       
       myScroll();  //move everything up a line so we can do the bottom one again
       break;
   }
        
}

void myScroll(){
  // We will move each line of text up one row
  
  int i = 0;  // we will use this variables in all our for loops
  
  ////lcd.setCursor(0,0);      // Move the cursor to the top left corner of the display
  Serial.print(line1);        // Print line1 here. Line1 is our second line,
                           // our top line is line0 ... on the next scroll
                           // we toss this away so we don't store line0 anywhere
 
  // Move everything stored in our line2 array into our line1 array
  for (i = 0; i < 20; i++) {
    line1[i] = line2[i];
  }
  
  //lcd.setCursor(0,1);      // Move the cursor to the beginning of the second line
  Serial.print(line1);        // Print the new line1 here
 
  // Move everything stored in our line3 array into our line2 array
  for (i = 0; i < 20; i++) {
    line2[i]=line3[i];
  }
  //lcd.setCursor(0,2);      // Move the cursor to the beginning of the third line
  Serial.print(line2);        // Print the new line2 here
 
  // Move everything stored in our currentLine array into our line3 array
  for (i = 0; i < 20; i++) {
    line3[i] = currentLine[i];
  }
 
}

void reprintOverFlow(){
  // Here we put the word that wouldn't fit at the end of the previous line
  // Back on the display at the beginning of the new line
  
  // Load up our current line array with what we have so far
   for (int i = 0; i < 20; i++) {
     currentLine[i] = lastWord[i];
  } 
  //lcd.setCursor(0, LCDline);              // Move the cursor to the beginning of our new line 
  Serial.print(lastWord);                    // Print the stuff we just took off the previous line
  letterCount = lastWordCount;            // Set up our character counter to match the text
  //lcd.setCursor(letterCount, LCDline); 
  lastSpace=0;          // clear the last space pointer
  lastWordCount=0;      // clear the last word length
}
