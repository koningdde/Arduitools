// WB7FHC's Morse Code Decoder v1.1 (modified for VT100) v2 with keyboard send by Barry Block (barryb@xs4all.nl) 1-2017
// v2 Jan 5, 2017, added keyboard send
// Jan 12, 2017 added code chart

const int audioPin = 0;          // *** mic preamp output to this pin. 0 for analog, 8 for digital
int audio = 1;                   // will store the value we read on this pin
unsigned int threshold = 0;      // *** audio input pin reference point for analog input relative to audioIn
unsigned int audioIn = 0;        // audioPin reference point when no audio
unsigned int ap;                 // audio pin reference
//const byte MorseOutPin = 11;   // visual/aural conformation of audio working
const byte spk = 8;
unsigned int downTime = 0;       // How long the tone was on in milliseconds
unsigned int upTime = 0;         // How long the tone was off in milliseconds
int myBounce = 2;                // Used as a short delay between key up and down
unsigned long startDownTime = 0;  // Arduino's internal timer when tone first comes on
unsigned long startUpTime = 0;    // Arduino's internal timer when tone first goes off
unsigned long lastDahTime = 0;    // Length of last dah in milliseconds
unsigned long lastDitTime = 0;    // Length of last dit in milliseconds
unsigned long averageDahTime = 0; // Sloppy Average of length of dahs
boolean justDid = true;           // Makes sure we only print one space during long gaps
int myNum = 0;                    // We will turn dits and dahs into a binary number stored here
const char mySet[] = "##TEMNAIOGKDWRUS##QZYCXBJP#L#FVH09#8###7#####/=61#######2###3#45"; // - changed to = //BB
char printChar;
boolean ditOrDah = true;          // We have either a full dit or a full dah
unsigned int dit = 60;//10;            // We start by defining a dit as 10 milliseconds
unsigned int averageDah = 180;//240;             // A dah should be 3 times as long as a dit - 15wpm
unsigned int averageWordGap = averageDah;  // will auto adjust
unsigned long fullWait = 6000;             // The time between letters
unsigned long waitWait = 6000;             // The time between dits and dahs
unsigned long newWord = 0;                 // The time between words
boolean characterDone = true;              // A full character has been sent
////////////////////////////////////////////////////////////////////////////

unsigned int wpm = 20;
boolean hear_speed = false;               // play wpm ... ---
unsigned int elementWait = 1200 / wpm;
boolean wpm_manual = true;               // over ride auto adjust wpm
String autoWPM = "on";                    // works with wpm_manual
unsigned int maxWPM = 90;
unsigned int minWPM = 10;
unsigned int try_wpm = 0;
unsigned long firstErr;
unsigned long tErr = 0;
unsigned long eTime = 4000;
//unsigned int spaceChr = 0;
boolean rSetMaxSpeed = false;

char keypress;
unsigned int value = 0;
boolean lower = true;
unsigned int err = 0;
unsigned int pitch = 630;        // *** 0 if using an oscillator, not tone(). You might want to use 600Hz, for example.
unsigned int default_p = pitch;  // for resetting pitch
unsigned int pitch_hold = pitch;
boolean pitch_on = true;
unsigned int soft = 540;         // *** find a pitch that is quieter in your setup
unsigned int loud = 800;         // *** find a pitch that is louder  in your setup
unsigned int space = 3;          // *** Default time between Morse characters.
unsigned int flag = 0;
signed char* b;
const String brag = "WB7FHC CW DECODER v1.1 (VT100 v3 B. Block) & keyboard send - press 'm' for menu ";
const String letters = "abcdefghijklmnopqrstuvwxyz";
const String numbers = "1234567890";
const String punctuation = ",.?!:\"'=@_()^&;+-$/";
const unsigned int array = 55;   // total number of Morse characters 0-54
char buffer[8];                  // make sure this is large enough for the largest code string it must hold. ($ == 7)
unsigned int tSend;              // used by send_all_characters() for testing

const char str0[] PROGMEM = "a";  const char str1[] PROGMEM = "b";  const char str2[] PROGMEM = "c";  const char str3[] PROGMEM = "d";
const char str4[] PROGMEM = "e";  const char str5[] PROGMEM = "f";  const char str6[] PROGMEM = "g";  const char str7[] PROGMEM = "h";
const char str8[] PROGMEM = "i";  const char str9[] PROGMEM = "j";  const char str10[] PROGMEM = "k"; const char str11[] PROGMEM = "l";
const char str12[] PROGMEM = "m"; const char str13[] PROGMEM = "n"; const char str14[] PROGMEM = "o"; const char str15[] PROGMEM = "p";
const char str16[] PROGMEM = "q"; const char str17[] PROGMEM = "r"; const char str18[] PROGMEM = "s"; const char str19[] PROGMEM = "t";
const char str20[] PROGMEM = "u"; const char str21[] PROGMEM = "v"; const char str22[] PROGMEM = "w"; const char str23[] PROGMEM = "x";
const char str24[] PROGMEM = "y"; const char str25[] PROGMEM = "z";

const char str26[] PROGMEM = "0"; const char str27[] PROGMEM = "1"; const char str28[] PROGMEM = "2"; const char str29[] PROGMEM = "3";
const char str30[] PROGMEM = "4"; const char str31[] PROGMEM = "5"; const char str32[] PROGMEM = "6"; const char str33[] PROGMEM = "7";
const char str34[] PROGMEM = "8"; const char str35[] PROGMEM = "9";

const char str36[] PROGMEM = ","; const char str37[] PROGMEM = ".";  const char str38[] PROGMEM = "?"; const char str39[] PROGMEM = "!";
const char str40[] PROGMEM = ":"; const char str41[] PROGMEM = "\""; const char str42[] PROGMEM = "'"; const char str43[] PROGMEM = "=";
const char str44[] PROGMEM = "@"; const char str45[] PROGMEM = "-";  const char str46[] PROGMEM = "("; const char str47[] PROGMEM = ")";
const char str48[] PROGMEM = "$"; const char str49[] PROGMEM = "&";  const char str50[] PROGMEM = ";"; const char str51[] PROGMEM = "+";
const char str52[] PROGMEM = "_"; const char str53[] PROGMEM = "^";  const char str54[] PROGMEM = "/";

// set up a table to refer to your strings.
const char* const strtable[] PROGMEM =
{
  str0, str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11, str12, str13, str14, str15, str16, str17, str18,
  str19, str20, str21, str22, str23, str24, str25, str26, str27, str28, str29, str30, str31, str32, str33, str34, str35, str36,
  str37, str38, str39, str40, str41, str42, str43, str44, str45, str46, str47, str48, str49, str50, str51, str52, str53, str54
};

// code strings // letters
const char code0[] PROGMEM = ".-";    const char code1[] PROGMEM = "-...";  const char code2[] PROGMEM = "-.-.";
const char code3[] PROGMEM = "-..";   const char code4[] PROGMEM = ".";     const char code5[] PROGMEM = "..-.";
const char code6[] PROGMEM = "--.";   const char code7[] PROGMEM = "....";  const char code8[] PROGMEM = "..";
const char code9[] PROGMEM = ".---";  const char code10[] PROGMEM = "-.-";  const char code11[] PROGMEM = ".-..";
const char code12[] PROGMEM = "--";   const char code13[] PROGMEM = "-.";   const char code14[] PROGMEM = "---";
const char code15[] PROGMEM = ".--."; const char code16[] PROGMEM = "--.-"; const char code17[] PROGMEM = ".-.";
const char code18[] PROGMEM = "...";  const char code19[] PROGMEM = "-";    const char code20[] PROGMEM = "..-";
const char code21[] PROGMEM = "...-"; const char code22[] PROGMEM = ".--";  const char code23[] PROGMEM = "-..-";
const char code24[] PROGMEM = "-.--"; const char code25[] PROGMEM = "--..";

// numbers
const char code26[] PROGMEM = "-----"; const char code27[] PROGMEM = ".----"; const char code28[] PROGMEM = "..---";
const char code29[] PROGMEM = "...--"; const char code30[] PROGMEM = "....-"; const char code31[] PROGMEM = ".....";
const char code32[] PROGMEM = "-...."; const char code33[] PROGMEM = "--..."; const char code34[] PROGMEM = "---..";
const char code35[] PROGMEM = "----.";

// punctuation
const char code36[] PROGMEM = "--..--";  const char code37[] PROGMEM = ".-.-.-"; const char code38[] PROGMEM = "..--..";
const char code39[] PROGMEM = "-.-.--";  const char code40[] PROGMEM = "---..."; const char code41[] PROGMEM = ".-..-.";
const char code42[] PROGMEM = ".----.";  const char code43[] PROGMEM = "-...-";  const char code44[] PROGMEM = ".--.-.";
const char code45[] PROGMEM = "-....-";  const char code46[] PROGMEM = "-.--.";  const char code47[] PROGMEM = "-.--.-";
const char code48[] PROGMEM = "...-..-"; const char code49[] PROGMEM = ".-...";  const char code50[] PROGMEM = "-.-.-.";
const char code51[] PROGMEM = ".-.-.";   const char code52[] PROGMEM = "..--.-"; const char code53[] PROGMEM = "..--";
const char code54[] PROGMEM = "-..-.";

const char* const codetable[] PROGMEM =
{
  code0, code1, code2, code3, code4, code5, code6, code7, code8, code9, code10, code11, code12, code13, code14, code15, code16,
  code17, code18, code19, code20, code21, code22, code23, code24, code25, code26, code27, code28, code29, code30, code31, code32,
  code33, code34, code35, code36, code37, code38, code39, code40, code41, code42, code43, code44, code45, code46, code47, code48,
  code49, code50, code51, code52, code53, code54
};

void setup()
{
  Serial.begin(115200); refresh_Screen();
  audioIn = analogRead(audioPin); threshold = audioIn - 10;
  Serial.println(brag); decoder_stats(); decoder_menu();
}

void loop()
{
  if (Serial.peek() != -1) getOption();

  // *** select digital or analog input depending on your input module. Audio goes up or down relative to threshold *** \\

  // This is for digital audio input only
  //unsigned int audioIn = digitalRead(audioPin); if (audioIn == HIGH) audio = false; else audio = true;

  // This is for analog input and (audioIn < threshold) audio = false
  audioIn = analogRead(audioPin);

  if (audioIn < threshold) audio = false; else audio = true;
  if (!audio) keyIsDown(); else keyIsUp();

  //  if (!audio) digitalWrite(MorseOutPin, HIGH); else digitalWrite(MorseOutPin, LOW); // LED display

}///////////////////////////////////////////////////////////////////////////////////////////////////////

void getOption()
{
  flag = 0; getChars(true); switch (keypress)
  {
    case 'c': refresh_Screen(); decoder_stats(); break;
    case 's': kboardSend(); break;
    case 'u': lower = !lower; Serial.print(F("case:")); if (lower) Serial.println(F("L")); else Serial.println(F("U")); break;
    case 'a': { // audio sensitivity
        ap = analogRead(audioPin);
        if (value == 1) threshold = ap - 10;      // default
        else if (value == 2) threshold = ap - 5;  // high
        else if (value == 3) threshold = ap - 40; // low
        else if (value > 3) threshold = value;
        String s = String(ap); Serial.println("\r\naudio_pin:" + s + " - threshold:" + String(threshold)); break;
      }
    case 'd': code_chart(); break;
    case 'p': setPitch(); break;
    case 'o': pitch_menu(); break;
    case 't': send_all(); break;
    case 'e': value = wpm; rSetMaxSpeed = !rSetMaxSpeed;
      Serial.print("\n\rset max speed:"); if (rSetMaxSpeed) Serial.println("on"); else Serial.println("off"); wpm = value; //value = 0;
    case  13: Serial.println(); break;
    case 'z': case ',': case '<': flag = 10; value = wpm - 10; setWPM(); break;
    case 'x': case '.': case '>': flag = 10; value = wpm + 10; setWPM(); break;
    case 'w': flag = 10; if (value != 0) setWPM();
      else {
        value = wpm;  setWPM(); wpm = value; // needed for auto speed or wpm drops to 10!
      }
      break;
    case 'v': wpm_manual = !wpm_manual; if (wpm_manual) autoWPM = "off";
      else {
        autoWPM = "on"; value = maxWPM; setWPM();
      }
      Serial.println("auto wpm:" + autoWPM); break;
    case 'm': case 27: refresh_Screen(); decoder_stats(); decoder_menu(); break;
    default: keypress = 0; Serial.println(F("unknown command")); break;
  }
  keypress = 0;
}

void decoder_menu()
{
  Serial.println(F("s keyboard send"));
  Serial.println(F("c clear screen"));
  Serial.println(F("d code chart"));
  Serial.println(F("u toggle upper/lower case"));
  Serial.println(F("a set audio sensitivity: a show, a1 default, a2 high, a3 low")); // only if analog input
  Serial.println(F("p set pitch"));
  Serial.println(F("o pitch menu"));
  Serial.println(F("t send all chars"));
  Serial.println(F("w display/set wpm, w1 hear wpm sample"));
  Serial.println(F("< or z wpm - 5"));
  Serial.println(F("> or x wpm + 5"));
  Serial.println(F("v toggle wpm_auto"));
  Serial.println(F("e toggle max speed auto"));
}

void keyIsDown() {
  digitalWrite(13, 1);
  if (startUpTime > 0) startUpTime = 0;
  if (startDownTime == 0) startDownTime = millis();
  characterDone = false;
  ditOrDah = false;
  delay(myBounce);
  if (myNum == 0) myNum = 1; // start of a new char?
}

void keyIsUp() {
  digitalWrite(13, 0);
  if (startUpTime == 0) startUpTime = millis();
  upTime = millis() - startUpTime;
  if (upTime < 10)return;
  if (upTime > (averageDah * 2)) printSpace();
  if (startDownTime > 0) {
    downTime = millis() - startDownTime;
    startDownTime = 0;
  }
  if (!ditOrDah) shiftBits();
  if (!characterDone) {
    if (upTime > dit) {
      printCharacter();
      characterDone = true;
      myNum = 0;
    }
    downTime = 0;
  }
}

void shiftBits() { ///////////////////////////////////////////////// bit shift
  if (downTime < dit / 3) return;
  myNum = myNum << 1;
  ditOrDah = true;
  if (downTime < dit) myNum++; else if (wpm_manual == false) setWPM();
}

void setWPM()
{
  if (value == 1) {
    hear_wpm(); return;
  }
  else if (value == 0 && flag != 10) // auto speed
  {
    averageDah = (downTime + averageDah) / 2; value = (3600 / averageDah);
  }
  wpm_chk();
}

void wpm_chk()
{
  if (value != 0) // not just asking what speed so set data
  {
    if (value < minWPM) wpm = minWPM; else if (value > maxWPM) wpm = maxWPM; else wpm = value;
    averageDah = 3600 / wpm; elementWait = 1200 / wpm; dit = averageDah / 3; dit = dit * 2;
  }
  if (flag == 10) // do not report if auto wpm and you did not ask
  {
    Serial.print(F("\n\rwpm:")); Serial.println(wpm); flag = 0;
    if (hear_speed) send_code("... ---");
  }
}

void hear_wpm()
{
  hear_speed = !hear_speed; Serial.print(F("hear wpm:"));
  if (hear_speed) Serial.println("on"); else Serial.println("off");

}

void printCharacter() {
  justDid = false;
  if ((myNum == 2 || myNum == 8) && !wpm_manual) startMaxSpeed();
  else if (myNum == 173 || myNum == 221) justDid = true; // cr-lf or clear_screen. no space wanted. BB
  if (myNum > 63) {
    printPunctuation();
    return;
  }
  printChar = mySet[myNum];
  if (mySet[myNum] == '#') printPunctuation();
  else sendToDisplay();
}

void printSpace() {
  if (justDid) return;
  justDid = true;
  averageWordGap = ((averageWordGap + upTime) / 2) + 20;
  printChar = ' ';
  sendToDisplay();
}

void printPunctuation() {
  switch (myNum) {
    case 28:
      printChar = '^'; break;
    case 41:
      printChar = '('; break;
    case 53:
      printChar = '+'; break;
    case 55:
      printChar = '&'; break;
    case 71:
      printChar = ':'; break;
    case 76:
      printChar = ','; break;
    case 82:
      printChar = ')'; break;
    case 84:
      printChar = '!'; break;
    case 85:
      printChar = ';'; break;
    case 94:
      printChar = '-'; break;
    case 97:
      printChar = '\''; break;
    case 101:
      printChar = '@'; break;
    case 106:
      printChar = '.'; break;
    case 109:
      printChar = '"'; break;
    case 114:
      printChar = '_'; break;
    case 115:
      printChar = '?'; break;
    case 246:
      printChar = '$'; break;
    case 122:
      printChar = 's'; sendToDisplay(); printChar = 'k'; break;
    case 173: Serial.println(); return; // -.-..-. bb new line (cr) return because, do not send to display
    case 221: refresh_Screen(); decoder_stats(); return;  // .-...-. bb clear screen (lr)
    default:  printChar = '#'; // Serial.print(String(myNum)); // test to find new chars to add   // BB
      if (!wpm_manual && rSetMaxSpeed) startMaxSpeed(); break;                 // if auto speed, start at max wpm // BB
  }
  sendToDisplay();
}

void sendToDisplay() {
  if (lower == true) printChar = toLowerCase(printChar); Serial.print(printChar);
}

void startMaxSpeed() // BB
{
  // This is to improve the auto speed which tends to hang on one long dah!
  // Auto works much better if it starts at max rather than min wpm.
  // This might not be the best place or best way to implement the idea but it's what I thought of.
  // Still working on this.

  if (printChar == '#') {
    err++;
    if (millis() - firstErr > 6000) { // long time since last error. reset
      err = 1; firstErr = millis();
    }
    else if (printChar == '#' && err > 2 && wpm < 15) {
      value = maxWPM; setWPM(); firstErr = millis();
    }
    else if (err > 3 && millis() - firstErr < 6000) { // for example. bump speed down from maxWPM
      try_wpm = try_wpm + 5; value = maxWPM - try_wpm; wpm_chk(); //err = 2;
    }
  }
  // if wpm is ~maxWPM and receiving ~40-10 wpm, the char 't' or 'o' tends to be 'decoded'. we trap this.
  else if (myNum == 2 || myNum == 8 || myNum == 4)
  {
    flag++;
    if (flag == 2 ) tErr = millis();
    else if (flag > 3 && millis() - tErr > eTime) flag = 0;
    else if (flag > 3 && millis() - tErr < eTime)
    {
      try_wpm = try_wpm + 10; if (try_wpm > 40) try_wpm = 0;
      if (wpm > 75)      value = maxWPM - try_wpm;
      else if (wpm < 15) value = minWPM + try_wpm;  // auto wpm hangs at 10 wpm
      wpm_chk(); flag = 1;
    }
  }
}

void decoder_stats() // BB
{
  refresh_Screen();
  Serial.print(brag); Serial.print(F("[aPin:")); ap = analogRead(audioPin); Serial.print(ap); Serial.print(F(" a:")); Serial.print(threshold);
  Serial.print(F(" p:")); Serial.print(pitch); Serial.print(F(" w:")); Serial.print(wpm); Serial.print(F(" v:"));
  if (wpm_manual) Serial.println(F("off]")); else Serial.println(F("on]"));
}

void refresh_Screen() // requires an ANSI terminal. can be toggled. see menu display
{
  Serial.write(27);    // ESC
  Serial.write("[2J"); // clear screen
  Serial.write(27);    // ESC
  Serial.write("[3J"); // clear scrollback
  Serial.write(27);    // ESC
  Serial.write("[H");  // cursor to home
}

void getChars(boolean k)
{
  int digits; value = 0;
  if (k == true)
  {
    do
    {
      digits = Serial.available();
    } while (digits < 1); // wait till a key is pressed.
    keypress = Serial.read(); keypress = tolower(keypress);
  }
  if (flag != 10)
  {
    do // collect the value if any
    {
      digits = Serial.available();
    } while (digits == -1);
    value = Serial.parseInt();
  }
}

///////////////////////////////////////////////// send from keyboard //////////////////////////////////

void kboardSend()
{
  flag = 10; String kSend = "keyboard send: \\ to enter a command - ESC to exit";
  Serial.println("\r\n" + kSend);
  while (true) {
    getChars(true);
    if (keypress == '\r') Serial.println();
    else if (keypress == ' ') delay(elementWait * 3);
    else if (keypress == 27) {                        // if ESC quit. go back to decoder
      clear_serial_buffer(); Serial.println(F("\r\ndecoder - s to send")); return;
    }
    else if (keypress == '\\') {                      // char '\' get command from decoder menu choices
      Serial.print(F("\n\rEnter command: "));
      while (Serial.available() < 1); keypress = Serial.peek();
      if (keypress == 's') Serial.println(F("\n\rAlready in keyboard send mode"));
      else {
        getOption(); flag = 10;
      }
    }
    else {
      Serial.print(keypress);
      for (unsigned int i = 0; i < array; ++i) {       // run through array to match char to send
        getStr(i); if (String(keypress) == buffer) {   // match found
          getCode(i); send_code(buffer); break;
        }
      }
    }
  }
}

void dit_dah(String x)                                 // actually send the sounds
{
  if (pitch != 0) tone(spk, pitch);
  if (x == ".") delay(elementWait);                    // one dit
  else if (x == "-") delay(elementWait * 3);           // dah 'length' is 3 dits
  noTone(spk); delay(elementWait);
}

void eWait()
{
  elementWait = 1200 / wpm;
}

void letter_space() {                                  // silence between chars
  delay(elementWait * space);
}


void send_code(String send_chars) // send the dits & dahs of the Morse string - codes[](Example: .-..) including spaces
{
  for (unsigned int i = 0; i < send_chars.length(); ++i)  // break the Morse char string down to it's elements and send
  {
    String x = send_chars.substring(i, i + 1);
    if (x == " ") letter_space();
    else dit_dah(x); // determine dit or dah and send
  }
  letter_space();
}

void setPitch()
{
  if (value > 0 && value < 6) return; if (value > 24000) return; if (value > 9 && value < 31) return;
  switch (value)
  {
    case 6: pitch = default_p; showP(); break;
    case 7: pitch = pitch - 10; if (pitch > 24000 || pitch < 31) pitch = 31; showP(); break; // unsigned int rollover
    case 8: pitch = pitch + 10; if (pitch > 24000) pitch = 24000; showP(); break;
    case 9: pitch = soft; showP(); break;
    case 99: pitch = loud; showP(); break;
    case 0: pitch_on = !pitch_on;
      if (!pitch_on) {
        pitch_hold = pitch; pitch = 0;
      }
      else pitch = pitch_hold; showP(); break;
    default: pitch = value;
  }
}

void pitch_menu()
{
  Serial.println(F("p(value) set pitch, p toggle pitch on/off, p6 default, p7 -10Hz, p8 +10Hz, p9 soft, p99 loud"));
}

void showP()
{
  Serial.print("pitch:"); Serial.println(pitch); if (pitch_on) send_code(".--.");
}

void getStr(unsigned int i) // copy to buffer a string (one char) stored in flash memory (a)
{
  strcpy_P(buffer, (char*)pgm_read_word(&(strtable[i])));
}

void getCode(unsigned int i) // copy to buffer a string stored in flash memory (.-)
{
  strcpy_P(buffer, (char*)pgm_read_word(&(codetable[i])));
}

void code_chart() // show all Morse chars on screen
{
  Serial.println(F("\n\rletters:"));
  Lchart();
  Serial.println(F("numbers:"));
  Nchart();
  Serial.println(F("punctuation:"));
  Pchart();
}

void Lchart()
{
  for (unsigned int i = 0; i < 13; ++i) {
    print_line(i);
  } Serial.println(F("\n"));
  for (unsigned int i = 13; i < 26; ++i) {
    print_line(i);
  } Serial.println(F("\n"));
}

void Nchart()
{
  for (unsigned int i = 26; i < 36; ++i) {
    print_line(i);
  } Serial.println(F("\n"));
}

void Pchart()
{
  for (unsigned int i = 36; i < 45; ++i) {
    print_line(i);
  } Serial.println(F("\n"));
  for (unsigned int i = 45; i < 55; ++i) {
    print_line(i);
  } Serial.println(F("\n"));
}

void print_line(unsigned int i) // with the above to display chart
{
  getStr(i); String c = String(buffer);
  if (!lower) c.toUpperCase();
  Serial.print("[" + c + "]");
  getCode(i); Serial.print(buffer); Serial.print(" ");
}

void send_all()
{
  for (tSend = 0; tSend < 26; tSend++) { // letters
    sendChars();
  }
  letter_space();
  for (tSend = 26; tSend < 36; tSend++) { // numbers
    sendChars();
  }
  letter_space();
  for (tSend = 37; tSend < array; tSend++) { // punctuation
    sendChars();
  }
  Serial.println(); send_code("-.-..-."); // carrage return. new line
}

void sendChars()
{
  if (Serial.peek() == 27) return;
  getCode(tSend); send_code(buffer);
}

void clear_serial_buffer()
{
  do {
    keypress = Serial.read();
  } while (Serial.available() > 0);
  keypress = 0; Serial.read();
}
