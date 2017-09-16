// Morse Receive & Send Practice Mega v4b
// by flat5 August 27, 2015 - Sept. 16, 2016
// UK/US characters only
// choice of tone/noTone or external oscillator circuit triggered by pin13.
// Plug speaker to Digital pin8 and Gnd. Use a resistor or pot in series if you want to. I do not.
// A few more punctuation characters added. Added a feature or two. Caught a few unwanted features.
// v2.5 added SD card. randomly chooses textfile to send as Morse
// This had pushed the Uno memory to the limit.
// version 2.6 is a rewrite of how static strings are stored. Only 51% of dynamic memory is used!
// NOTE - number of textfiles in SD root folder must be named 0 1 2...98...127...999
// format to Fat32 & avoid 'too many files' error. array will not work with 4 digit numbers unless YOU change it
// v2.7 & v2.8 Some SD card options added. This ver, some code cleanup and a feature or two added.
// v2.8.a changed the behaviour of some options
// v2.8.b added - $ to the char* arrays. added array (size) variable to simplify adding new chars
// v2.9 adding a feature from Morse Machine. computer sends a char. you type the char correctly or computer sends it again
// v.2.9a altered a routine that calls itself by using a label (goto) to improve stability
// v.2.9.b a few tweaks and corrections
// v2.9.c Morse Machine now does not repeat chars. sends string and makes a new one
// v2.9.d New Morse Machine routine added for the punctuation characters. Send two of them x times each then add a third character...
// v2.9.e generalize Morse Machine 'add a char' for letters, numbers, punctuation. some code cleanup
// v2.9.f strings, letters, numbers, punctuation now const strings.
//        more string cleanup to get room for an SD card Help text file routine.
// v3 added bookmark to z1 z2 z3 routine. continue where you left off
// v4 send practice added using iambic paddle. Program renamed.
// text strings edited to gain bytes to add paddle routine. Sorry, more cryptic :-(
// Paddle routine now allows for wpm adjustment by holding dit or dah for 20 counts. +2 or -2. you can change this
// (Paddle) Code is sent to confirm new speed. No keyboard or usb needed! portable (+ battery)
// Also, a 100k resistor should be connected between vin and A5. This is to recognize if the board is being powered by a battery.
// If so, the program expects that no keyboard or monitor is connected and you just want to practice sending.
// ver 4.2 code cleanup and new feature. Esc from sending file. Screen text more crytic to save bytes.
// ver 4.31 changed some routines. added word space adjustment. added '/' -..-. to array. now 55 chars
// ! changed to "-.-.--";
// Further developement now will be for the Mega.
// Mega v1.1. added routines. Improved helpfile. not done yet.
// Mega v1.3 for Mega has many changes and enhancements. Straight key function added. Rewrite of SDcard functions.
// Mega v2 now includes Budd Churchward's (WB7FHC) Morse Code Decoder v.1.1
// modified by BB to remove the LCD routines and add manual wpm control.
// http://mypetarduino.com/MorseduinoSketch2.3.htm // this sketch uses v1.1
// This requires a mic module if you want audio input. I highly recogmmend this small, cheap, sensitive module:
// http://www.banggood.com/CJMCU-9812-MAX9812L-Electret-Microphone-Amplifier-Development-Board-Sensor-Module-For-Arduino-p-1103128.html
// v3 basic keyer memory added to paddle routine
// v4 upgraded keyer, saving data to SDcard. other updates. t-hunt beacon added. start in beacon mode added. pToggle added. Sept. 15, 2016
// pseudo call sign generator added Oct. 20, 2016
// link: https://1drv.ms/u/s!AiSSerzJFFhhfa4tlY5odjb1WMk
// 28-11-16 typein teststring added as option. version now v4a
// a little code clean up and bug fixes. version now v4b. more code cleanup Dec 21,25,28, 2016
// cr/lf and clr added to decoder for fun. cr = -.-..-. clr = .-...-.
// it is expected that you use a VT-100 or better terminal program at this point. I've dropped support for dumb terminals.
// a little code clean up and bug fixes. version now v4b. more code cleanup Jan 1, 2017
// keyboard send option added Jan 2, 2017. more code cleanup
// Jan 3, 2017 answer check (t8) added after suggestion by Maarten Stuurman
// Jan 5, 2017 code cleanup. Jan 12, 2017 menu added to keyboard_send
// https://1drv.ms/u/s!AiSSerzJFFhhgQ4RhLG-V9MIivEo

#include <SPI.h>
#include <SD.h>
#define P_dit pDit   // D3 Connects to the dit lever of the paddle (or the other way around)
#define P_dah pDah   // D2 Connects to the dah lever of the paddle
unsigned int pDit = 2;
unsigned int pDah = 3;
// keyer
#define recordMax 10      // number of keyer messages
#define messageLength 255 // max length of a keyer message
char f[9]; File ID; File dir; File entry; String text; unsigned int Kfiles = 0; char k_dir[] = "/keyer/"; // SD subfolder
//
const String letters = "abcdefghijklmnopqrstuvwxyz"; // 0-25
const String numbers = "1234567890";                 // 26-36
const String punctuation = ",.?!:\"'=@_()^&;+-$/";   // 37-54
char buffer[8];                  // make sure this is large enough for the largest code string it must hold. ($ == 7)
unsigned int tSend;              // used by send_all() characters for testing
const byte chipSelect = 53;      // SD card select
boolean echo = true;             // echo text as it is being sent or after sending complete file

char hfile[] = "/utility/MRP_Help.txt"; // *** a Help text file. Put it on the SD card using this suggested folder/name.
char fname[25];
char bFile[] = "/utility/beacon.txt";
const unsigned long int baud = 115200; // *** 115200 57600 9600 300 2 make sure the terminal program matches!
boolean SDready = true;          // *** SD card installed, true or false. set to false if no SD card to avoid error message
boolean ANSI = true; //false     // *** If you are using an ANSI terminal program (VT-100)
boolean scrollback = true;       // *** When you clear screen this will also clear the scrollback buffer if set to 'true'
boolean lower = true;            // *** display letters as lower or upper case
//                               //     at the beginning of some lines change this from true to false. now a menu item
unsigned int pitch = 760;        // *** 0 if using an oscillator, not tone(). You might want to use 600Hz, for example.
unsigned int default_p = pitch;  // for resetting pitch
unsigned int soft = 580;         // *** find a pitch that is quieter in your setup
unsigned int loud = 780;         // *** find a pitch that is louder  in your setup
unsigned int space = 3;          // *** Default time between Morse characters.
//                               //     It's good to send characters faster than the space between them for practice.
unsigned int wSpace = 1;         // *** time between words. 1 is normal
unsigned int wpm_talkback = 13;  // *** wpm speed the computer tells you what speed paddle is set to
const byte signalPin = 13;       // *** will control oscillator/speaker, if using external circuit. else connect sounder to D8
const byte relayPin = 9;         // *** output pin for relay follows state of signal pin, if enabled. simple keyer. might follow up on this
boolean relay = false;           // *** relay default condition
unsigned int wpm = 20;           // *** Change this to any default value
boolean hear_speed = false;      // play wpm ... ---
const unsigned int array = 55;   //     total number of Morse characters 0-54
unsigned int row = array;        // *** do NOT make this number larger than the array!
boolean tRepeat = false;          // *** test repeat if testrow is 1. Useful to learn a new char. You can toggle this in the program
unsigned int tTime = 1000;       // *** wait time for tRepeat in milliseconds
unsigned int modulo = 4;         // *** if you use hundreds of files in the two file groups on the SDcard change this number to 10 or 6

// WB7FHC's Morse Code Decoder v.1.1 stuff//////////////////////////////////////////////////////////////////////////////////////////
const int audioPin = 0;          // *** mic preamp output to this pin
unsigned int threshold;          // audio threshold level
boolean audio = false;           // audio on audioPin or not
unsigned int downTime = 0;       // How long the tone was on in milliseconds
unsigned int upTime = 0;         // How long the tone was off in milliseconds
int myBounce = 2;                // Used as a short delay between key up and down
unsigned long startDownTime = 0; // Arduino's internal timer when tone first comes on
unsigned long startUpTime = 0;   // Arduino's internal timer when tone first goes off
unsigned long lastDahTime = 0;   // Length of last dah in milliseconds
unsigned long lastDitTime = 0;   // Length of last dit in milliseconds
unsigned long averageDahTime = 0; // Sloppy Average of length of dahs
boolean justDid = true;          // Makes sure we only print one space during long gaps
int myNum = 0;                   // We will turn dits and dahs into a binary number stored here
const char mySet[] = "##TEMNAIOGKDWRUS##QZYCXBJP#L#FVH09#8###7#####/=61#######2###3#45"; // - changed to = //BB
char printChar;
const String brag = "WB7FHC CW DECODER v1.1 - press c to clear screen - m for menu - ESC to quit ";
boolean ditOrDah = true;          // We have either a full dit or a full dah
unsigned int dit = 12;            // We start by defining a dit as 12 milliseconds (10)
unsigned int averageDah = 36;     // A dah should be 3 times as long as a dit - 100wpm (15)
String h = "off";                 // status of hear_speed

// The following values will auto adjust to the sender's speed
boolean wpm_manual = true;                // over ride auto adjust wpm
unsigned int averageWordGap = averageDah;  // will auto adjust
unsigned long fullWait = 6000;             // The time between letters
unsigned long waitWait = 6000;             // The time between dits and dahs
unsigned long newWord = 0;                 // The time between words
boolean characterDone = true;              // A full character has been sent
// end decoder vars /////////////////////////////////////////////////////////////////////////////////////

unsigned int Jfiles = 0, Afiles = 0, wpm_hold = wpm, testStrRow = 0, flag = 0, mmRepeat = 5, tmp_flag, delay_repeat = 4000,
             type_ans_len = 0, pitch_hold = pitch, value, err, randNumber, elementWait;
String Cbak = "", L_case = "L", send_char, answers, code_string = "", sendfile,  t4 = "off", t5 = "off", t6 = "off", t7 = "off", t8 = "off",
       kr = "off", ti = "off", tStr, filename, testStr, typeIn, eko;
boolean battery = false, jokes = false, linefeed = false, tStringRepeat = false, typeAnswer = false, pitch_on = true, show_busy;
File root, ARRL, KEYER; unsigned long lastTone = 0; char keypress;

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
  eWait(); noTone(8);
  pinMode(P_dit, INPUT_PULLUP); pinMode(P_dah, INPUT_PULLUP); // paddle
  pinMode(signalPin, OUTPUT); // tone output pin. To control an oscillator
  pinMode(relayPin, OUTPUT);  // To enable/disable & control a relay (keyer)
  pinMode(A5, INPUT); // usb or battery power? if battery go to paddle(), stright_key() or beacon
  Serial.begin(baud); randomSeed(analogRead(1)); refresh_Screen();
  if (SDready)
  {
    Serial.print("SDcard? ");
    if (!SD.begin(chipSelect))
    {
      SDready = false; Serial.println("- no SDcard"); delay(3000);
    }
    else {
      if (analogRead(A5) > 950) batteryPower(); // usb == 909-928. battery == 940-1024 my system. weak battery produced a long tone
      if (!SD.exists(k_dir)) SD.mkdir(k_dir);
      Serial.print("Yes. Counting SD files"); SDready = true; show_busy = true;
      flag = 1; countFiles(root, Jfiles); flag = 2; countFiles(ARRL, Afiles); flag = 3; countFiles(KEYER, Kfiles);
      flag = 0; show_busy = false; delay(400);
    }
  }
  threshold = analogRead(audioPin) - 10; // default audio threshold level
  toggles_text(); // set default text for boolean options
  menu();
}

void loop()
{
  parser();
}

void menu() {
  refresh_Screen();
  //unsigned int n = analogRead(A5); Serial.println(n); wpm = n; wpm2code(); // usb/battery test
  //if battery check is not working, adjust the numbers in setup()
  Serial.println(F("Receive/Send code practice v4b for Arduino Mega by Barry Block (b5@xs4all.nl) + WB7FHC's Morse Code Decoder v.1.1\n"));
  Serial.println(F("Press Spacebar to start test - ESC to stop\n"));
  Serial.println(F("s(value) set char spacetime (1-255) -- o(value) set word spacetime (1-30) (wS"));
  //Serial.println(F("o(value) set word spacetime (1-30)"));
  Serial.println(F("w(value) display (set) wpm -- w1 toggle hear_wpm"));
  Serial.println(F("p(value) set pitch, p toggle pitch on/off, p6 default, p7 -10Hz, p8 +10Hz, p9 soft, p99 loud"));
  Serial.println(F("r(value) set number of chars to send (row length: 1-55)"));
  if (!SDready) Serial.println(F("k toggle key relay on/off"));
  Serial.println(F("t Enter chars to send or just Enter to clear 'testString' or ESC to abort option"));
  Serial.println(F("t4 toggle tString char repeat -- t5 seconds to wait -- t6 toggle tString auto repeat"));
  Serial.println(F("t7 toggle typein tString answers -- t8 with t7, toggle repeat till correct"));
  if (SDready)
  {
    Serial.println(F("t1 (Load) t2 (Save) t3 (Delete) testString"));
    Serial.println(F("k toggle key relay on/off -- k (1-8) keyer menu -- k9 beacon mode"));
    Serial.println(F("f1 (send joke) or f2 (arrl text)"));
    Serial.println(F("x SDcard menu -- h read helpfile"));
    //Serial.println(F("h helpfile"));
    Serial.println(F("e toggle text echo on/off while receiving a text file"));
  }
  Serial.println(F("z computer sends testString char, you press matching key"));
  Serial.println(F("z1(letters) z2(numbers) z3(punctuation) z>3(# of group repeats) - to learn the code."));
  Serial.println(F("n(row) send numbers -- l(row) send letters -- u(row) send punctuation or u77 to send letters/numbers only"));
  //Serial.println(F("l(row) send letters"));
  //Serial.println(F("u(row) send punctuation or u77 to send letters/numbers only"));
  Serial.println(F("v toggle upper/lower case letters"));
  //Serial.println(F("g toggle dit/dah paddles"));
  Serial.println(F("y send using paddle or y(value) using straight key -- g toggle dit/dah paddles"));
  Serial.println(F("c clear screen or toggle option:c(1-8) c9:toggle scrollback clear (Scl"));
  Serial.println(F("d show code chart"));
  Serial.println(F("b audio decoder & straight key send with char display"));
  Serial.println(F("j pseudo call sign generator"));
  Serial.println(F("a keyboard send"));
  Serial.println(F("m menu"));
  Serial.print("Stats: e:" + eko); Serial.print(" k:" + kr); Serial.print(" Scl:" + Cbak); Serial.print(" case:" + L_case);
  Serial.print(" t4:" + t4); Serial.print(" t5:" + t5); Serial.print(" t6:" + t6); Serial.print(" t7:" + t7); Serial.print(" t8:" + t8);
  Serial.print(" s:"); Serial.print(space); Serial.print(" wS:"); Serial.print(wSpace); Serial.print(" w:");
  Serial.print(wpm); Serial.print(" h:" + h); Serial.print(" p:"); Serial.print(pitch); Serial.print(" r:"); Serial.print(row);
  Serial.print(" Zx:"); Serial.print(mmRepeat);
  if (Jfiles > 0) {
    Serial.print(F(" Jfiles:")); Serial.print(Jfiles);
  }
  if (Afiles > 0) {
    Serial.print(F(" Afiles:")); Serial.print(Afiles);
  }
  if (Kfiles > 0) {
    Serial.print(F(" Kfiles:")); Serial.print(Kfiles);
  }
  if (testStr != "") {
    Serial.print(F("\n\rtRow:")); Serial.print(testStrRow); Serial.print(F(" tStr ")); Serial.print(tStr);
  }
  Serial.println();
}

void parser()
{
  flag = 0; getChars(true); //refresh_Screen();
  switch (keypress)
  {
    case ' ': // Spacebar. generate & send code
      random_code(); break;
    case 'a': kboardSend(); break;
    case 'b': decoder(); menu(); break;
    case 'c': // clear screen if ANSI is supported or toggle feature
      if (value > 0 && value < 9) ANSI = !ANSI;
      else if (value == 9)
      {
        ANSI = true;
        scrollback = !scrollback;
        if (scrollback == false) Cbak = "off"; else Cbak = "on";
        Serial.println("Cbak:" + Cbak);
      }
      else refresh_Screen();
      break;
    case 'd': // display code chart
      refresh_Screen(); code_chart(); break;
    case 'e': // toggles text echo. display chars on screen just after they are sent.
      echo = !echo;
      if (echo == true)
      {
        Serial.println ("Echo:on"); eko = "on";
      }
      else
      {
        Serial.println ("Echo:off"); eko = "off";
      }
      break;
    case 'f': // send random textfile from SD card
      refresh_Screen();
      if (value == 1) {
        flag = 1; send_SDtext();
      }
      else if (value == 2) {
        flag = 2; send_SDtext();
      }
      else {
        Serial.println(F("options: 1 or 2"));
      }
      break;
    case 'g': pToggle(); break;
    case 'h': case '?': // show program help file
      if (SDready)
      {
        if (SD.exists(hfile)) // display '/utility/MRP_Help.txt'
        {
          refresh_Screen();
          File dataFile = SD.open(hfile, FILE_READ);
          while (dataFile.available()) Serial.write(dataFile.read());
          dataFile.close();
        }
        else Serial.println("\r\n/utility/MRP_Help.txt not found");
      }
      break;
    case 'j': call_generator(); break;
    case 'k':
      keyer_option(); break;
    case 'l': // send the letter characters
      testStr = letters; tStr = "letters"; testStrRow = 26;
      if (value > 0) testStrRow = value;
      if (testStrRow > 26)
      {
        Serial.println(F("1-26 chars")); testStrRow = 26;
      }
      Serial.print("\ntString " + tStr + " - "); row_message(); break;
    case 'm': case 'i': // display menu
      menu(); break;
    case 'n': // send the number characters
      testStr = numbers; tStr = "numbers"; testStrRow = 10;
      if (value > 0) testStrRow = value;
      if (testStrRow > 10)
      {
        Serial.println(F("1-10 chars")); testStrRow = 10;
      }
      Serial.print("\ntString " + tStr + " - "); row_message(); break;
    case 'o':
      if (value > 0 && value < 31) wSpace = value;
      Serial.print("wSpace:"); Serial.println(wSpace); break;
    case 'p': setPitch(); break; // pitch
    case 'r': // how many chars to send (a row)
      if (value == 0)
      {
        Serial.print(F("row:")); Serial.println(row);
        if (testStr != "")
        {
          Serial.print(F("tString row:")); Serial.println(testStrRow);
        }
        break;
      }
      if (testStr != "")
      {
        if (value <= testStr.length())
        {
          testStrRow = value; row_message();
        }
        else Serial.println(F("Value > testString"));
        break;
      }
      else if (value <= array)
      {
        row = value; Serial.print("row:"); Serial.println(row); break;
      }
      else
      {
        Serial.println(F("Value too high")); break;
      }
    case 's': // letter space time length (1 is same time as a dit)
      if (value != 0) space = value; // value 0 not allowed. just report setting
      Serial.print(F("Letter space:")); Serial.println(space);
      break;
    case 't': // send a selected group of characters
      if (value == 4)
      {
        tRepeat = !tRepeat; if (tRepeat == true) t4 = "on"; else t4 = "off"; Serial.println("tRepeat:" + t4);
      }
      else if (value == 5) testPause(); // seconds to wait before repeating a char or run. works with t4 & t7
      else if (value == 6)
      {
        tStringRepeat = !tStringRepeat; if (tStringRepeat == true) t6 = "on"; else t6 = "off";
        Serial.print(F("tStringRepeat:")); Serial.println(t6);
      }
      else if (value == 7) // Enter chars after teststring test
      {
        if (!typeAnswer) {
          if (testStrRow > 10) {
            refresh_Screen(); Serial.println(F("testString is > 10 characters. Do you really want 'typeAnswer' on? y/n"));
            flag = 10; getChars(true); flag = 0;
            if (keypress == 'n') {
              Serial.println(F("typeIn: off"));
            }
          }
        }
        typeAnswer = !typeAnswer;
        if (t7 == "off") {
          t7 = "on"; tStringRepeat = false; t6 = "off"; tRepeat = false; t4 = "off"; // t4 and t6 conflict with t7
        }
        else {
          t7 = "off"; t8 = "off"; // t8 must be off if t7 is off
        }
        Serial.println("typeIn:" + t7);
      }
      else if (value == 8) {
        if (t7 == "off") break; if (t8 == "off") t8 = "on"; else t8 = "off"; Serial.println("typeIn check:" + t8);
      }
      else if (value > 0 && value < 4 && SDready) SD_tString();
      else if (value == 0) {
        build_testStr(); tStr = testStr;
      }
      break;
    case 'u': // send the punctuation characters or not
      if (value > 76)
      {
        testStr = numbers + letters; tStr = "nums&lets"; Serial.print("\ntString " + tStr + " - "); testStrRow = 36; row_message(); break;
      }
      testStr = punctuation; tStr = "punctuation"; Serial.print("\ntString " + tStr + " - "); testStrRow = 19;
      if (value > 0) testStrRow = value;
      if (testStrRow > 19)
      {
        Serial.println(F("1-19 chars")); testStrRow = 19;
      }
      row_message(); break;
    case 'v':
      lower = !lower;
      Serial.print("\n\rletter case:");
      if (lower == true) {
        Serial.println("lower"); L_case = "L";
      }
      else {
        Serial.println("upper"); L_case = "U";
      }
      break;
    case 'w': WPM(); break; // wpm
    case 'x': // SD card sub menu
      if (SDready) SDfilesMenu(); break;
    case 'y': // practice sending with a paddle
      if (value != 0) straight_key(); else paddle(); break;
    case 'z': // a crude 'Morse Machine'
      if (value > 3)
      {
        mmRepeat = value; Serial.print(F("Will repeat ")); Serial.print(mmRepeat); Serial.println(F("x then add a char"));
      }
      else flag = value; Morse_Machine();
      break;
    default: break;
  }
}

void random_code() // send a random character's Morse equivalent and display answer. Heart of the program!
{
  if (testStr != "") // characters have been selected so just send them. Idea is to always use the Spacebar to send a test run
  {
    send_select(); return;
  }
  for (unsigned int t = 0; t < row; ++t) // each run will be 'row' characters (+ 'row' spaces)
  {
    if (Serial.read() == 27) // if ESC quit sending. go back to menu
    {
      Lcase(); return; // show test so far & quit
    }
    randNumber = random(array); // 0 - 54, all characters.
    // see if the randNumber character is in string 'answers' already.
    getStr(randNumber); // in it's way, returns 'buffer' which is the char string pointed to by the index 'randNumber'
    if (answers.indexOf(buffer) != -1) t--; // character already in array. reject and try again
    else
    { // answer string is built fast (even 55 chars). no jerkyness so no need to pre-build a code array to send
      answers.concat(buffer);
      getCode(randNumber); send_code(buffer); // get the code string & send it (.-..)
    }
  }
  Lcase(); // show test answers and clear string
}

void Lcase() // select lower/upper letter case for 'answers' string, display & clear. called 4 places
{
  if (!lower) answers.toUpperCase();
  int n = row; if (testStrRow != 0) n = testStrRow;
  if (typeAnswer && testStrRow != 0) Serial.print(F("test answers: "));
  for (int i = 0; i < n; i++)
  { // reduces answers string by half
    Serial.print(answers.charAt(i)); Serial.print(" "); // more stable than 1 print statement
  }
  Serial.println(); if (flag != 15) answers = ""; // needed for t8 routine
}

void send_select() // called when testStr has proved legal
{
  if (keypress == 27)
  {
    type_ans_len = 0; menu(); return;
  }
  String code, Str;
  if (testStrRow == 0) testStrRow = testStr.length();
_repeat:
  for (unsigned int t = 0; t < testStrRow; ++t) // random up the string to be sent
  {
    randNumber = random(testStr.length());
    if (answers.indexOf(testStr[randNumber]) != -1) t--; // character already in array. try again
    else
    {
      answers.concat(testStr[randNumber]); // add to string
    }
  }
t8_repeat:
  if (keypress == 27) {
    menu(); return;
  }
  unsigned int t = 0;
  while (t < testStrRow)
  {
    for (unsigned int i = 0; i < array; ++i) // run through array to match char to send
    {
      if (Serial.read() == 27) // if ESC quit sending. go back to parsor()
      {
        Lcase(); type_ans_len = 0; answers = ""; return;
      }
      getStr(i);
      if (answers.substring(t, t + 1) == buffer) // match found
      {
        Str = buffer; getCode(i); send_code(buffer); // get char code pointed to by i and send the code
        code = buffer;                               // for the tRepeat routine below
        if (typeAnswer) type_tstring();
        break;
      }
    }
    t++;
  }
  word_space();
  if (t8 == "off") Lcase();
  else {
    if (typeIn.equalsIgnoreCase(answers) == false) {
      delay(900); goto t8_repeat;
    }
    else Lcase();
  }
  if (testStr == punctuation) Serial.println(); // puncs look better if they are 'run' seperated
  repeatChar(code, Str); // special routine if tString == 1 and tRepeat:on
  if (typeAnswer)
  {
    type_ans_len = 0;
    if (keypress == 27) {
      menu(); return;
    }
    keypress = 0; delay(tTime); send_select();
  }
  else if (tStringRepeat) {
    delay(tTime); goto _repeat;
  }
} // end send_select()

void type_tstring() // option - type in testString after a test run. Only works with a testString!
{
  refresh_Screen();
  type_ans_len++; // do nothing till all Morse chars are sent (tString)
  if (type_ans_len == testStrRow) // now enter your answers (testStrRow is tString.length()
  {
    type_ans_len = 0; flag = 10; typeIn = ""; String typeIn_space; unsigned int i = 0;
    Serial.println("Enter your answers or Space if you don't know a char - Enter to repeat - ESC to quit");
    do
    {
      if (keypress == 27) break; if (keypress == '\r') {
        break;
      }
      i++; getChars(true); // returns char keypress
      typeIn = typeIn + keypress;                   // need a string without spaces for t8 string compare
      typeIn_space = typeIn_space + keypress + " "; // need a string with spaces for display
    } while (i < testStrRow && keypress != 27);
    if (keypress == 27) {
      type_ans_len = 0; answers = ""; return;
    }
    if (keypress != '\r' ) {
      if (!lower) typeIn.toUpperCase(); Serial.println("your answers: " + typeIn_space);
    }
    clear_serial_buffer();
  }
} // back to send_select()

void repeatChar(String code, String Str) // special routine if tString == 1 and tRepeat:on
{
  if (testStrRow == 1 && tRepeat == true)
  {
    refresh_Screen(); Serial.println(F("Spacebar for next character - ESC to exit"));
    while (true)
    {
      if (Serial.peek() == 27) { // quit, show menu
        menu(); return;
      }
      if (Serial.peek() == 32) { // quit. send another (hopefully different) char
        return; send_select();
      }
      clear_serial_buffer();
      send_code(code); delay(tTime); if (!lower) Str.toUpperCase(); Serial.println(Str + "  " + code);
    }
  }
}

void testPause() // seconds to wait before repeating a char or run. works with t4, t6, and t7
{
  Serial.println("How many seconds to wait? (0-9)");
  flag = 10; getChars(true); flag = 0;
  if (isdigit(keypress)) {
    tTime = ((int(keypress) - 48) * 1000); toggles_text(); Serial.println("t5:" + t5 + " seconds");
  }
  else Serial.println("not a number");
}

void build_testStr()   // Select characters for study (option t)
{
  inString_build();    // collect chars and put in testStr if they are legal. if not, return flag 1
  if (flag == 1)
  {
    flag = 0; return;
  }
  if (testStr != "")   // testStr is legal
  {
    Serial.print(F("tString ")); Serial.println(testStr);
  }
}

void inString_build() // get chars from user and check them. if ok add them to testStr
{
  Serial.println(F("Input chars & Enter to create tString or just Enter to clear it or ESC to quit"));
  testStr = collect_chars(testStr); //get user input
  if (testStr == "")
  {
    Serial.println(F("tString cleared")); return;
  }
  // now test the string for problems
  for (unsigned int t = 0; t < testStr.length(); ++t) // check for duplicates
  {
    char h; h = char(testStr.charAt(t)); // overcoming the string vs char compile error by CASTING
    for (unsigned int i = t + 1; i < testStr.length(); ++i)
    {
      if (h == char(testStr.charAt(i)))
      {
        Serial.print(testStr.substring(t, t + 1)); Serial.println(F(" <--Repeated char - tString cleared"));
        flag = 1; testStr = ""; return;
      }
    }
  }
  for (unsigned int s = 0; s < testStr.length(); ++s) // check for not Morse char
  {
    flag = 1; // set the flag false
    for (unsigned int i = 0; i < array; ++i)          // check to see that the character is a known Morse char.
    {
      getStr(i);
      if (testStr.substring(s, s + 1) == buffer)      // if a match is found for this char set flag 'true'. don't check further
      {
        flag = 0; break;
      }
    }
    if (flag == 1) // illegal character. kill testStr and return. don't check rest of testStr.
    {
      Serial.print(testStr.substring(s, s + 1)); Serial.println(F(" <--unknown Morse char - tString cleared"));
      testStr = ""; return;
    }
  }
}

String collect_chars(String testStr) // user input to build testStr
{
  String tmp = testStr; testStr = ""; int trw = testStrRow; testStrRow = 0; unsigned int n = flag; flag = 10; // get keypress but not value
  while (keypress != '\r')
  {
    getChars(true); testStr = testStr + keypress; Serial.print(keypress);
    if (keypress == 27) {
      testStr = tmp; testStrRow = trw; flag = n; return testStr;
    }
  }
  Serial.print(keypress); testStr.trim(); if (testStrRow == 0) testStrRow = testStr.length(); flag = n; return testStr;
}

void send_code(String send_chars) // send the dits & dahs of the Morse string - codes[](Example: .-..) including spaces
{
  for (unsigned int i = 0; i < send_chars.length(); ++i) // break the Morse char string down to it's elements and send
  {
    String x = send_chars.substring(i, i + 1);
    if (x == " ") letter_space();
    else dit_dah(x); // determine dit or dah and send
  }
  letter_space();
}

void WPM()
{
  if (value == 1) {
    hear_wpm(); return;
  }
  else if (value > 7)
  {
    wpm = value; eWait(); averageDah = 3600 / wpm;
  }
  if (flag != 14) { // keyboard send
    Serial.print(F("\n\rwpm:")); Serial.println(wpm); if (hear_speed) send_code("... ---");
  }
}

void eWait()
{
  elementWait = 1200 / wpm;
}

void dit_dah(String x) // actually send the sounds
{
  digitalWrite(signalPin, HIGH); if (relay == true) digitalWrite(relayPin, HIGH); // enable external oscillator and/or relay
  if (pitch != 0) tone(8, pitch);
  if (x == ".") delay(elementWait); // milliseconds == one dit
  else if (x == "-") delay(elementWait * 3); // dah 'length' is 3 dits
  digitalWrite(signalPin, LOW); digitalWrite(relayPin, LOW); // disable external oscillator and/or relay.
  noTone(8); delay(elementWait); lastTone = millis(); // record when last tone was sent for paddle/straight key routine
}

void letter_space() { // silence between chars
  delay(elementWait * space);
}

void word_space() {  // length of silence between words
  delay((elementWait * 6 * wSpace) + 1);
}

void clear_serial_buffer()
{
  do {
    keypress = Serial.read();
  } while (Serial.available() > 0);
  keypress = 0; Serial.read();
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

void getStr(unsigned int i) // copy to buffer a string (one char) stored in flash memory (a)
{
  strcpy_P(buffer, (char*)pgm_read_word(&(strtable[i])));
}

void getCode(unsigned int i) // copy to buffer a string stored in flash memory (.-)
{
  strcpy_P(buffer, (char*)pgm_read_word(&(codetable[i])));
}

// SDcard functions //////////////////////////////////////////////////////////// SDcard functions

void SDfilesMenu() // SD file options menu & routines
{
  refresh_Screen();
  Serial.println("Choose SD folder submenu");
  Serial.println(F("\nJoke, ARRL, Helpfile? (j/a/h)\n"));
  flag = 10; getChars(true); flag = 0;
  if (keypress == 'a') ARRLfiles();
  else if (keypress == 'j') Jokes();
  else if (keypress == 'h') Helpfile();
  menu();
}

void AJ_menu()
{
  Serial.println(F("l list files"));
  Serial.println(F("r(file) read file"));
  Serial.println(F("s(file) send file"));
  Serial.println(F("> keep sending random files"));
  Serial.println(F("d(file) delete file"));
  Serial.println(F("w(file) write file"));
  Serial.println(F("\\ main menu command"));
  Serial.println(F("ESC exit to main menu"));
}

void Jokes()
{
  refresh_Screen();
label_j:
  Serial.println(F("\n\rJoke file options:\n")); AJ_menu(); getChars(true); refresh_Screen();
  filename = String(value); filename.toCharArray(fname, 5);
  flag = 1; // for countfiles() and send SDtext()
  switch (keypress)
  {
    case 'd': delete_file("/", "joke"); countFiles(root, Jfiles); goto label_j;
    case 'l': filename = String(Jfiles); printDirectory("/", filename); goto label_j;
    case 'r': read_textfile(fname); goto label_j;
    case 's': sendfile = filename; send_SDtext(); goto label_j;
    case '>': value = 200; send_SDtext(); goto label_j;
    case 'w': file_write(fname); countFiles(root, Jfiles); goto label_j; //value
    case '\\': Serial.print(F("enter command:\n\r")); parser(); goto label_j;
    case 27 : return;
    default: goto label_j;
  }
}

void ARRLfiles()
{
  refresh_Screen();
label_a:
  Serial.println(F("\nARRL file options:\n")); AJ_menu(); getChars(true); refresh_Screen();
  filename = "ARRL/" + String(value) + ".TXT"; filename.toCharArray(fname, 15); flag = 2; // for countfiles and send SDtext()
  switch (keypress)
  {
    case 'd': delete_file("/ARRL/", "arrl"); countFiles(ARRL, Afiles); goto label_a;
    case 'l': filename = String(Afiles); printDirectory("/ARRL/", filename); goto label_a;
    case 'r': read_textfile(fname); goto label_a;
    case 's': sendfile = filename; send_SDtext(); goto label_a;
    case '>': value = 200; send_SDtext(); goto label_a;
    case 'w': file_write(fname); countFiles(ARRL, Afiles); goto label_a;
    case '\\': Serial.println(F("\n\renter command:\n\r")); parser(); goto label_a;
    case 27 : return;
    default:  goto label_a;
  }
}

void Helpfile()
{
  Serial.print(F("Update helpfile? y/n \n\rThis will delete the present: ")); Serial.println(hfile);
  getChars(true);
  if (keypress != 'y') return;
  value = 9999; filename = "/utility/MRP_Help.txt"; filename.toCharArray(fname, 22); file_write(fname);
}

void send_SDtext() // send and show a random text file stored on the SD card
{
  clear_serial_buffer();
  String str;
  if (sendfile != "")
  {
    str = sendfile; sendfile = ""; // a file has been specifically chosen
  }
  else if (flag == 1)
  {
    unsigned int rnd = random(Jfiles); str = String(rnd); // convert int to string
  }
  else if (flag == 2)
  {
    unsigned int rnd = random(Afiles); str = "/ARRL/" + String(rnd) + ".TXT";
  }
  Serial.print(F("file:")); Serial.print(str); Serial.println(F(" - Spacebar to pause - ESC to abort\n\r"));
  File dataFile = SD.open(str, FILE_READ);
  if (dataFile)
  {
    String send_char; int SDint; String printChars;
    while (dataFile.available() && Serial.peek() != 27)
    {
      if (Serial.read() == ' ') while (Serial.read() == -1); // press Spacebar for pause
      SDint = dataFile.read();
      char c = char(SDint);    // int to char
      send_char = String(c);   // char to string
      printChars = send_char;  // keep the cap letters for screen printout
      send_char.toLowerCase(); // array is lower case
      for (unsigned int i = 0; i < array; ++i) // check to see that the character is a known Morse char, space, or new line
      {
        if (send_char == " " || send_char == "\n") // space, or new line
        {
          word_space(); break;
        }
        else if (send_char < " " || send_char > "z") break; // avoid non Morse chars
        getStr(i);
        if (send_char == buffer) { // if a match is found for this char, get code and send it
          getCode(i); send_code(buffer); break;
        }
      }
      if (echo) // It's fun to see the text as the code is being sent.
      {
        if (!lower) printChars.toUpperCase(); Serial.print(printChars);
      }
    }
    dataFile.close(); if (value != 200) clear_serial_buffer();
    refresh_Screen(); Serial.println(str + "\n\r"); // clear screen & show filename
    File dataFile = SD.open(str, FILE_READ);
    while (dataFile.available())
    {
      char b = dataFile.read(); if (!lower) b = toupper(b); Serial.print(b);
    }
    dataFile.close(); Serial.println();
  }
  else // if the file isn't open, pop up an error:
  {
    Serial.print(F("not found: ")); Serial.println(str);
  }
  if (value == 200 && Serial.peek() != 27) { // wait 5 seconds, clear screen, continue to send random files
    delay(5000); send_code(".-...-."); send_SDtext();
  }
  else clear_serial_buffer();
}

unsigned int countFiles(File, unsigned int) // get number of text files in SD dir
{
  File dir;
  if (flag == 1) {
    Jfiles = 0; dir = SD.open("/");
  }
  if (flag == 2) {
    Afiles = 0; dir = SD.open("ARRL/");
  }
  if (flag == 3) {
    Kfiles = 0; dir = SD.open("keyer/");
  }
  dir.rewindDirectory(); // Begin at the start of the directory
  while (true)
  {
    File entry = dir.openNextFile(); if (!entry) break;
    String n = entry.name();
    if (n.toInt() != 0 || n == "0" || n == "0.TXT") //only files starting with number BUT file 0 will not display unless exception is made
    {
      if (flag == 1) {
        ++Jfiles; if (Jfiles > 999) Jfiles = 999; // have to change array char string size for a bigger (4 digit) number
        if (show_busy) if (Jfiles % modulo == 0) Serial.print("."); // a little bootup show
      }
      else if (flag == 2) {
        ++Afiles; if (Afiles > 999) Afiles = 999; // have to change array char string size for a bigger (4 digit) number
        if (show_busy) if (Afiles % modulo == 0) Serial.print(".");
      }
      else if (flag == 3) {
        ++Kfiles; if (Kfiles > recordMax) {
          Serial.println(); Serial.print(Kfiles); Serial.println(F(" messages found!"));
          Serial.print(recordMax); Serial.println(F(" Allowed. Please delete extra files."));
        }
        if (show_busy) if (Kfiles % modulo == 0) Serial.print(".");
      }
      entry.close();
    }
  }
  if (flag == 1) return Jfiles; else if (flag == 2) return Afiles; else if (flag == 3) return Kfiles; else return 0;
}

void SD_tString()
{
  File myFile; char u[] = "utility"; char t[] = "utility/t1";
  if (value == 1) // read file
  {
    myFile = SD.open(t);
    if (myFile)
    {
      testStr = "";
      while (myFile.available()) // read from the file until there's nothing else in it
      {
        char a = myFile.read(); testStr = testStr + a; // read a char and add to testStr
      }
      myFile.close(); tStr = testStr; testStrRow = testStr.length(); Serial.println(F("tString loaded"));
    }
    else
    {
      Serial.println(F("\nError opening /utility/t1\n")); // if the file didn't open, print an error
    }
    return;
  }
  else if (value == 2 && testStr != "") // write file
  {
    if (!SD.exists(u)) SD.mkdir(u);
    SD.remove(t); myFile = SD.open(t, FILE_WRITE);
    if (myFile) { // if the file opened okay, write to it
      myFile.print(testStr); myFile.close(); Serial.println(F("tString Saved")); // write the string to the file & close it
    }
    else Serial.println(F("error opening /utility/t1")); return;
  }
  else if (value == 2 && testStr == "") {
    Serial.println(F("No testString to save")); return;
  }
  else if (value == 3) // delete file
  {
    Serial.println(F("!! Delete tString from SDcard? y/n")); getChars(true);
    if (keypress == 'y') {
      SD.remove(t); delay(50); // just seemed like a good idea
      if (!SD.exists(t)) Serial.println(F("File deleted"));
    }
    else Serial.println(F("Not deleted"));
  }
}

void file_write(char fname[])
{
  if (value == 0) {  // until a better solution is found
    Serial.println(F("File 0 protected")); return;
  }
  if (SD.exists(fname)) {
    Serial.print("File: "); Serial.print(fname); Serial.println(" exists. Overwrite it? y/n");
    getChars(true);
    if (keypress != 'y') {
      refresh_Screen(); return;
    }
    SD.remove(fname); Serial.println("removed");
  }
  Serial.println("Add linefeeds? y/n"); getChars(true); if (keypress == 'y') linefeed = true; else linefeed = false;
  File dataFile = SD.open(fname, FILE_WRITE);
  refresh_Screen(); Serial.println("File: " + filename); Serial.print(F("Waiting..."));
  delay(10); unsigned int i = 0; byte inChar = ' '; // ' ' removes compiler warning
  while (Serial.peek() < 1);
  Serial.println(F("receiving data"));
  do
  {
    if (Serial.peek() > 0)
    {
      inChar = Serial.read(); if (inChar < 127) dataFile.write(inChar);
      if (inChar == 13 && linefeed) dataFile.write(10); // add linefeed
      ++i; if (i > 100) {
        Serial.print("."); i = 0; // show something is happening
      }
    }
  } while (inChar != 4 && inChar != 129 && Serial.peek() != 27); // 'end of' or ESC
  dataFile.close();
  Serial.println("\n\r" + filename + " saved");
  clear_serial_buffer();
}

void delete_file(String path, String group)
{
  if (value == 0) {
    Serial.println(F("File 0 protected\n"));  return;
  }
  refresh_Screen(); Serial.println("File:" + filename);
  clear_serial_buffer();
  Serial.println("Delete file? y/n");
  getChars(true); refresh_Screen();
  if (keypress != 'y') return;
  if (SD.exists(fname)) {
    File dataFile = SD.open(path);
    SD.remove(fname);
    Serial.println("File:" + filename + " Deleted");
    dataFile.close();
    if (group == "joke") countFiles(dataFile, Jfiles); else if (group == "arrl") countFiles(dataFile, ARRL);
  }
  else Serial.println(F("not found"));
}

void read_textfile(char fname[])
{
  refresh_Screen(); Serial.println("File:" + filename);
  if (SD.exists(fname))
  {
    Serial.println();
    File dataFile = SD.open(fname, FILE_READ);
    while (dataFile.available()) Serial.write(dataFile.read());
    dataFile.close();
  }
  else Serial.println(F("not found"));
  Serial.println();
}

void printDirectory(String path, String fGroup) // display the names and size of the text files in SD folder or root
{
  // show files in numeric order
  unsigned int i = 0; // we want 5 columns of file/size
  if (ANSI == true) Serial.println(F("File\tSize\t\tFile\tSize\t\tFile\tSize\t\tFile\tSize\t\tFile\tSize"));
  Serial.println(); value = 0;
  while (true)
  {
    String fl; fl = path + String(value); if (flag == 2) fl = fl + ".txt";
    File entry = SD.open(fl); if (!entry) value++; if (value > entry.size()) break;
    String n = entry.name();
    //if (n.toInt() != 0 || n == "0" || n == path + "0.TXT") // check that filename is a number
    {
      Serial.print(entry.name()); Serial.print(F("\t")); Serial.print(entry.size(), DEC); Serial.print(F("\t\t"));
      ++i;
      if (i > 4) // 5 columns so new line
      {
        Serial.println(); i = 0;
      }
    }
    entry.close(); value++;
  }
  root.close(); value = 0; Serial.print(F("\r\nFiles:")); Serial.println(fGroup);
} ///////////////////////////////////////////////////////////////////////////////// end SDcard functions

void getChars(boolean k) // get user input. option and value if any
{
  int digits; value = 0;
  if (flag != 9 && ANSI == true && flag != 10) Serial.print("<"); // gives some indication the prog hasn't crashed & awaiting instructions
  if (k == true)
  {
    do
    {
      digits = Serial.available();
    } while (digits < 1); // wait till a key is pressed. 1 == a key press
    keypress = Serial.read(); keypress = tolower(keypress); // we now have used input, char (keypress) and in lower case
  }
  if (flag != 10)
  {
    do // collect the value if any
    {
      digits = Serial.available();
    } while (digits == -1); // -1 is returned until a key is pressed
    value = Serial.parseInt();
  }
  if (ANSI == true && flag != 10 && flag != 9) Serial.write(8); // backspace. remove prompt '<'
}

void confirm_pitch() // play a sample of the new pitch
{
  Serial.print("Hz:" + String(pitch)); tone(8, pitch, 400); // give a sample
}

void toggles_text()
{
  if (scrollback) Cbak = "on"; else Cbak = "off";
  if (echo) eko = "on"; else eko = "off";
  if (!tRepeat) t4 = "off"; else t4 = "on";
  t5 = String(tTime / 1000);
}

void Morse_Machine() // computer sends code, one char. user presses appropriate key
{
  wpm_hold = wpm; wpm = 25; eWait(); // store wpm. set wpm for this routine.
  String holdStr = testStr, mmStr, c;
  unsigned int u = 0, x = 0, h = 0, v = 2; // start with two chars
  filename = "/utility/mmData" + String(flag); filename.toCharArray(fname, 22);
  if (flag > 0 && flag < 4 && SDready)
  {
    if (SD.exists(fname)) // load bookmark for z1, z2, z3 routine
    {
      File dataFile = SD.open(fname, FILE_READ);
      byte buf[2];
      for (unsigned int i = 0; i < 2; ++i) buf[i] = dataFile.read();
      v = buf[0]; x = buf[1]; // v = number of chars in teststring so far. x = number of repeats so far
      dataFile.close();
    }
  }
  refresh_Screen(); Serial.println(F("ESC to quit - Spacebar for answer\n"));
  if (flag == 1) {
    mmStr = letters; Lchart();  // show relevent part of code_chart()
  }
  else if (flag == 2) {
    mmStr = numbers; Nchart();
  }
  else if (flag == 3) {
    mmStr = punctuation; Pchart();
  }
  h = mmStr.length();
  if (testStr == "" && flag == 0)
  {
    code_chart();
    for (unsigned int i = 0; i < array; ++i) // build string of all chars
    {
      getStr(i); testStr = testStr + buffer;  // string of chars to work with
    }
    h = testStr.length();
  }
  else if (testStr != "" && flag == 0) h = testStr.length();
label_m:
  if (flag > 0 )          // special handling of routine if selected - z(number)
  {
    ++x;                  // count chars sent
    testStr = mmStr.substring(0, v);
    if (x > v * mmRepeat) // each char sent 'mmRepeat' times (till x). default is 5
    {
      ++v;                // add another char to testStr
      x = 1;              // reset the test counter
    }
  }
  answers = "";
  for (unsigned int t = 0; t < testStr.length(); ++t)    // random up the string to be sent
  {
    randNumber = random(testStr.length());
    if (answers.indexOf(testStr[randNumber]) != -1) t--; // character already in array. try again
    else answers.concat(testStr[randNumber]);
  }                                                      // answers is testStr randomized. ready to send each char one at a time
  u = 0;                                                 // u is char position in answer string
  while (true)                                           // k == s so get next char to send
  {
    if (u == testStr.length()) goto label_m;       // spent 2 days trying not to use goto. sorry I did. best way to get out of a nest
    for (unsigned int i = 0; i < array; ++i)
    {
      String a = answers.substring(u, u + 1);           // get and send a char
      getStr(i);
      if (a == buffer)
      {
        String s = buffer; // Morse char
        getCode(i);
        c = buffer; // Morse string
        while (true)                             // need a 'while' to break out of
        {
          send_code(c); getChars(true); String k = String(keypress);
          if (v > h) k = "\e"; // test finished!
          if (keypress == 126) k = "^"; // special keyboard/terminal problem with ^ and _
          if (keypress == 127) k = "_"; // don't know why. just happy I found this
          if (k == s)                   // user input correct. send another char
          {
            ++u; ++x; break;
          }
          if (k == " ")                 // show hint, and not too subtle :-)
          {
            String z = String(s); if (!lower) z.toUpperCase();
            Serial.print("[" + z + "]  " ); Serial.println(c);
          }
          if (k == "\e") // cleanup, save some data, quit and return to parser()
          {
            if (SDready && flag > 0)
            {
              Serial.println(F("\nBookmark: Save y/n or Delete d"));
              getChars(true);
              switch (keypress)
              {
                case 'd':
                  {
                    SD.remove(fname); break; // Don't bother to check if exist. if file does not exist, who cares?
                  }
                case 'y':
                  {
                    char subdir[] = "utility/";
                    if (!SD.exists(subdir)) SD.mkdir(subdir);
                    SD.remove(fname);
                    File dataFile = SD.open(fname, FILE_WRITE);
                    dataFile.write(v); dataFile.write(x);  // not much of a file but if you just spent an hour taking the test...
                    dataFile.close();
                    break;
                  }
              }
            }
            answers = ""; testStr = holdStr; wpm = wpm_hold; eWait(); menu(); return;
          }
        }
      }
    }
  }
} ////////////////////////////////////////////////////////////////////////////////// end Morse_Machine()

void paddle() // send code w/option to change wpm without keyboard/monitor
{
  unsigned int dit = 0, dah = 0; unsigned long lastChar = 0; wpm_hold = wpm;
  refresh_Screen(); flag = 4; Key_msg();
  while (Serial.peek() != 27)
  {
    while (digitalRead(P_dit) && digitalRead(P_dah) && Serial.peek() == -1) // wait
    {
      if (!battery)
      {
        lastChar = millis() - lastTone;
        if (lastChar > elementWait * 3 + 1 && lastChar < elementWait * 6 && code_string != "") match_code_str();
      }
    }
    if (!digitalRead(P_dit)) // If the dit pin is LOW...
    {
      dit_dah(".");         // send a dot and update lastTone
      if (!battery) // in battery mode there is no screen or keyboard attached
      {
        ifWord(lastChar); // add space between words
        code_string.concat(".");
      }
      if (battery) // if 15 dits with no dah, wpm = wpm + 2 for example
      {
        ++dit; dah = 0; // if 15 dits with no dah, we have a change wpm request
        if (dit > 14)
        {
          dit = 0; wpm = wpm + 2; eWait(); wpm2code(); // update data & send the new wpm to user
        }
      }
    }
    if (!digitalRead(P_dah)) // If the dah pin is LOW...
    {
      dit_dah("-");         // send a dash
      if (!battery) // in battery mode there is no screen or keyboard attached
      {
        ifWord(lastChar);
        code_string.concat("-");
      }
      if (battery)
      {
        ++dah; dit = 0;     // if 15 dahs with no dits, we have a change wpm request
        if (dah > 14)
        {
          dah = 0; wpm = wpm - 2; eWait(); wpm2code(); // update data & send the new wpm to user
        }
      }
    }
    if (Serial.peek() != -1) {
      kPress(); if (flag == 11) {
        code_string = ""; wpm = wpm_hold; eWait(); menu(); return;
      }
      Key_msg();
    }
  }
}

void ifWord(unsigned long lastChar)
{
  if (lastChar >= elementWait * 7 && code_string == "") Serial.print(" ");
}

void match_code_str()
{
  boolean chk = false; // will we find char in array getCode(j)?
  for (unsigned int j = 0; j < 54; ++j)
  {
    getCode(j); // database of chars in code (--..) returns 'buffer'
    if (code_string == buffer) // string of dits & dahs
    {
      getStr(j); // get matching char (z)
      String c = String(buffer);
      if (!lower) c.toUpperCase();
      Serial.print(c);
      chk = true; // char found
      break;
    }
  }
  if (!chk) Serial.println("\nError");
  code_string = "";
}

void wpm2code() // send the new wpm to user in code
{
  unsigned int Save_wpm = wpm;                  // save wpm
  String w = String(wpm);                       // int to string for wpm_talkback. to send wpm as code
  wpm = wpm_talkback;                           // wpm for wpm_talkback. you can change this at top of sketch
  eWait(); delay(400);                          // give ear a break before sending the number. you can change this
  for (unsigned int s = 0; s < w.length(); ++s) // for each char of string
  {
    for (unsigned int i = 26; i < 36; ++i)      // find number in string array
    {
      getStr(i);
      if (w.substring(s, s + 1) == buffer)      // if a match is found
      {
        getCode(i);                             // grab code of the number
        send_code(buffer);                      // send the code of the digit
        delay(400);                             // pause between digits. you can change this
        break;
      }
    }
  }
  wpm = Save_wpm; eWait(); // reset wpm
}

void straight_key()
{
  unsigned int e = elementWait; unsigned long kdn = 0, kup = 0, t = 0, w = 0; boolean key = false; wpm_hold = wpm;
  refresh_Screen(); Key_msg();
  while (Serial.peek() != 27)
  {
    key = false; w = millis();
    while (!digitalRead(P_dit))
    {
      if (!key) kdn = millis(); key = true;
      digitalWrite(signalPin, HIGH); if (relay) digitalWrite(relayPin, HIGH); if (pitch != 0) tone(8, pitch);
    }
    noTone(8); digitalWrite(signalPin, LOW); digitalWrite(relayPin, LOW);
    if (key)
    {
      if (w - kup >= e * 3)   Serial.print(" ");  // space between chars
      if (w - kup >= e * 6.5) Serial.print(" ");  // space between words (2 spaces)
      kup = millis(); t = kup - kdn;
      if (t > e / 1.5 && t < e * 2.5 )      Serial.print(".");
      else if (t >= e * 2.6 && t < e * 5.5) Serial.print("-");
    }
    if (Serial.peek() != -1)
    {
      kPress(); if (flag == 11) {
        wpm = wpm_hold; eWait(); menu(); return;
      }
      Key_msg();
    }
  }
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

void showP()
{
  Serial.print("pitch:"); Serial.println(pitch); if (pitch_on) send_code(".--.");
}

void send_wpm() // send "wpm:" then wpm
{
  send_code(".-- .--. -- ---..."); // "wpm:"
  word_space(); String WPM = String(wpm);
  for (unsigned int i = 0; i < WPM.length(); ++i)
  {
    String S = (WPM.substring(i, i + 1 ));
    for (unsigned int j = 0; j < array; ++j)
    {
      getStr(j);
      if (S == buffer)
      {
        getCode(j); send_code(buffer); break;
      }
    }
  }
  delay(1500); clear_serial_buffer();
}

void hear_wpm()
{
  hear_speed = !hear_speed; if (hear_speed) h = "on"; else h = "off";
  Serial.print(F("hear wpm:")); Serial.println(h);
}

void Key_msg()
{
  String p = ""; if (flag == 4) p = "Paddle: g pToggle - "; else p = "Stright key: ";
  String cls = p + "Space speed sample - w wpm - c clr screen - p pitch - k keyer - l mList - s# mSend - ESC exit ";
  refresh_Screen();  Serial.print(cls); Serial.println("[wpm:" + String(wpm) + " p:" + String(pitch) + "]");
}

void row_message()
{
  Serial.print(F("row:")); Serial.println(testStrRow);
}

void kPress() // paddle & straight key
{
  getChars(true);
  switch (keypress)
  {
    case ' ': send_wpm(); return;
    case 'p': setPitch(); return;
    case 'w': WPM(); return;
    case 'c': return;
    case 'k': keyer(); return;
    case 27:  flag = 11; return;
    case 's': send_message(value); return;
    case 'l': list_files(); while (Serial.available() < 1); return;
    case 'g': pToggle(); return;
    default: return;
  }
}
// keyer functions follow ////////////////////////////////////////
void keyer()
{
  refresh_Screen();
  while (true)
  {
    Kmenu(); getChars(true); if (keypress == 27) return;
    k_parser();
  }
}

void k_parser()
{ refresh_Screen();
  if (keypress != 'l' && keypress != 'm' && keypress != 'x' && keypress != 'b' && keypress != 'w' && keypress != 'k') if (value == 0) return;
  switch (keypress)
  {
    case 's': send_message(value); break;
    case 'l': list_files(); break;
    case 'c': create_message(value); break;
    case 'r': read_message(value); break;
    case 'd': deleteOneMessage(value); break;
    case 'b': beaconMode(); break;
    case 'w': WPM(); break;
    case 'k': if (value == 0) keyer_option(); break;
    case 'p': setPitch(); break;
    case 'x': deleteAll(); break;
    case 'm': break;
    default: break;
  }
}

void Kmenu()
{
  Serial.println(F("\n\rKeyer Options:"));
  Serial.println(F("s# send message"));
  Serial.println(F("c# create/replace message"));
  Serial.println(F("r# read message"));
  Serial.println(F("d# delete message"));
  Serial.println(F("w# set wpm"));
  Serial.println(F("p# set pitch, p0 toggle pitch on/off"));
  Serial.println(F("l  list messages"));
  Serial.println(F("x  delete all messages!"));
  Serial.println(F("b  beacon mode"));
  Serial.println(F("k  toggle keyer relay"));
  Serial.println(F("m  show this menu"));
  Serial.println(F("ESC exit keyer"));
}

void create_message(unsigned int value)
{
  tmp_flag = flag; flag = 3; countFiles(KEYER, Kfiles);
  if (Kfiles > recordMax || value > recordMax)
  {
    Serial.print(recordMax); Serial.println(F(" messages allowed")); return;
  }
  Serial.print(F("Creating message ")); Serial.println(value);
  collect_chars(); text = text + "\0";
  test_text(); if (flag == 1) {
    flag = tmp_flag; return; // any non Morse chars?
  }
  get_filename(value); SD.remove(f); // don't want to append or edit
  ID = SD.open(f, FILE_WRITE); ID.print(text); ID.close(); Serial.println(); flag = tmp_flag;
}

void send_message(unsigned int)
{
  tmp_flag = flag; flag = 5; read_message(value); flag = tmp_flag;
}

void read_message(unsigned int value)
{
  if (value < 1) return;
  else if (value < 11)
  {
    get_filename(value); if (!SD.exists(f)) return;
    Serial.println(); Serial.print(value); Serial.print(") ");
    ID = SD.open(f, FILE_READ);
  }
  else if (value == 12)
  {
    if (!SD.exists(bFile)) return;
    ID = SD.open(bFile, FILE_READ);
  }
  while (ID.available()) // we have the file selected and open
  {
    char b = ID.read(); if (b == '\0') break; // not necessary when things go right :-)
    String c = String(b); String u = c; if (!lower) u.toUpperCase(); Serial.print(u);
    if (flag == 5) { // send message
      for (unsigned int i = 0; i < array; ++i) // check to see that the character is a known Morse char, space, or linefeed
      {
        if (Serial.read() == 27) {
          ID.close(); flag = 11; return;
        }
        else if (c == " " || c == "\n")
        {
          word_space(); break;
        }
        else if (c < " " || c > "z") break;
        getStr(i);
        if (c == buffer) { // if a match is found for this char, get code and send it
          getCode(i); send_code(buffer); break;
        }
      }
    }
  }
  ID.close(); Serial.println();
}

void deleteOneMessage(unsigned int value)
{
  get_filename(value); if (!SD.exists(f)) return;
  Serial.print(F("\n\rDeleting message: ")); Serial.println(value);
  SD.remove(f);
}

void get_filename(unsigned int value) // returns filename 'f'
{
  if (value == 0) return; String s = "/keyer/" + String(value); // as setup, length is max 8
  for (unsigned int i = 0; i <= 8; i++) f[i] = s[i]; // char arrays & string arrays both start at 0
}

void list_files()
{
  countFiles(KEYER, Kfiles);
  for (unsigned int i = 1; i <= recordMax; i++)
  {
    get_filename(i); if (SD.exists(f)) read_message(i);
  }
}

void deleteAll()
{
  Serial.println(F("\n\rDelete all messages? y/n"));
  getChars(true); if (keypress != 'y')
  {
    Serial.println("delete aborted"); return;
  }
  for (unsigned int i = 1; i <= recordMax; i++)
  {
    get_filename(i); SD.remove(f);
  }
  Serial.println(F("messages deleted"));
}

void collect_chars() // user input to build txt
{
  text = ""; int n = flag; flag = 10; // get 'keypress' but not 'value'
  Serial.println(F("Enter text"));
  while (keypress != '\r')
  {
    getChars(true); text = text + keypress; Serial.print(keypress);
  }
  text.trim(); flag = n;
}

unsigned int test_text()
{
  if (text.length() > messageLength)
  {
    Serial.println(F("\n\rMessage too long! Try again")); flag = 1; return flag;
  }
  for (unsigned int s = 0; s < text.length(); ++s) // check for non Morse char
  {
    flag = 1; // set the flag false
    for (unsigned int i = 0; i < array; ++i) // check to see that the character is a known Morse char.
    {
      getStr(i);
      if (text.substring(s, s + 1) == buffer || text.substring(s, s + 1) == " ") // if a match set a flag, good. don't check further
      {
        flag = 0; break;
      }
    }
    if (flag == 1) // illegal character. kill text and return. don't check rest of text.
    {
      Serial.print(text.substring(s, s + 1)); Serial.println(F(" <--unknown Morse char - message create aborted"));
      text = ""; return flag;
    }
  }
  return flag;
}

void send_string(String mess)
{
  for (unsigned int i = 0; i < mess.length(); i++)
  {
    for (unsigned int j = 0; j < array; j++)
    {
      String S = mess.substring(i, i + 1); S.toLowerCase(); //charAt() did not work ??
      getStr(j);
      if (String(buffer) == S)
      {
        if (!lower) S.toUpperCase();
        Serial.print(S); getCode(j); send_code(String(buffer)); break;
      }
      else if (S == " ")
      {
        word_space(); Serial.print(" "); break;
      }
    }
    if (Serial.read() == 27) return;
  }
}

// beacon func. //////////////////////////////////////////////////////
void beaconMode()
{
  unsigned int n = wpm; wpm = 25; eWait(); refresh_Screen();
  while (Serial.read() != 27)
  {
    Serial.println(F("\n\rBeacon mode - ESC to quit\n\r"));
    Serial.println(F("Options:"));
    Serial.println(F("c create/update"));
    Serial.println(F("r read"));
    Serial.println(F("s send"));
    Serial.println(F("d delete"));
    Serial.println(F("w# wpm"));
    Serial.println(F("k toggle keyer relay"));
    Serial.println(F("p# set pitch, p0 toggle pitch on/off"));
    Serial.println(F("y# repeat delay in seconds"));
    getChars(true); refresh_Screen();
    switch (keypress)
    {
      case 'c': create_beacon(); break;
      case 'r': read_beacon(); break;
      case 's': send_beacon(); break;
      case 'd': delete_beacon(); break;
      case 'w': WPM(); break;
      case 'k': if (value == 0) keyer_option(); break;
      case 'p': setPitch(); break;
      case 27: wpm = n; eWait(); return;
    }
  }
}

void create_beacon()
{
  Serial.println(F("c create/update:"));
  collect_chars(); text = text + "\0";
  test_text(); if (flag == 1) return; // any non Morse chars?
  SD.remove(bFile); // don't want to append or edit
  ID = SD.open(bFile, FILE_WRITE);
  ID.print(text); ID.close(); Serial.println();
}

void read_beacon()
{
  value = 12; read_message(value);
}

void send_beacon()
{
  if (!SD.exists(bFile)) {
    Serial.println(F("beacon textfile not found")); return;
  }
  else Serial.println(F("sending beacon - ESC to quit"));
  do
  {
    value = 12; flag = 5; read_message(value); if (flag == 5) delay(delay_repeat); // on exit flag is reset to 0
  } while (flag != 0 && flag != 11); // if ESC is pressed 'read_message(value)' will set flag to 0
}

void delete_beacon()
{
  Serial.println(F("delete beacon y/n"));
  getChars(true); if (keypress != 'y') return;
  SD.remove(bFile);
}
// end beacon func.

void pToggle() // toggle paddle assignments
{
  if (pDit == 2) {
    pDit = 3; pDah = 2;
  }
  else {
    pDit = 2; pDah = 3;
  }
  Serial.println(F("paddle toggled"));
}

void call_generator()
{
  refresh_Screen(); Serial.println(F("Pseudo Call Generator - ESC to quit"));
  do
  {
    answers = "";
    value = random(25); send_call_char(); value = random(25); send_call_char(); value = random(26, 35); send_call_char();
    value = random(25); send_call_char(); value = random(25); send_call_char(); value = random(25); send_call_char();
    if (!lower) answers.toUpperCase(); Serial.println(answers); delay(tTime);
  } while (Serial.read() != 27);
  answers = "";
}

void send_call_char()
{
  getStr(value); answers = answers + buffer; getCode(value); send_code(buffer);
}

void keyer_option()
{
  if (value == 0) {
    relay = !relay; if (relay == true) kr = "on"; else kr = "off"; Serial.println("keyRelay:" + kr);
  }
  else if (SDready) {
    if (value == 9) {
      beaconMode(); menu();
    }
    else if (value > 0 && value < 9 && SDready) {
      keyer(); menu();
    }
  }
}

void refresh_Screen() // requires an ANSI terminal. can be toggled. see menu display
{
  if (ANSI == true)
  {
    Serial.write(27);    // ESC
    Serial.write("[2J"); // clear screen
    Serial.write(27);    // ESC
    if (scrollback == true) Serial.write("[3J"); // clear scrollback
    Serial.write(27);    // ESC
    Serial.write("[H");  // cursor to home
  }
  else Serial.println();
}

void batteryPower()
{
  battery = true;  unsigned long dt = 0; unsigned long ut = 0;
  while (digitalRead(P_dit)); // wait
  dt = millis(); // start key down time
  while (!digitalRead(P_dit)) {
    tone(8, 685); ut = millis() - dt; // key up time
  }
  noTone(8); //Serial.print("time "); Serial.println(ut); delay(1000); // testing, still deciding times
  // How long you hold the key down determines which procedure is called.
  // paddle, stright key, or beacon. Short time: paddle. medium time: straight key. long time: beacon.
  if (ut < 500) {
    wpm2code(); paddle();
  }
  else if (ut > 499 && ut < 1000) straight_key();
  else if (ut > 1500) {
    pitch = 0; relay = true; send_beacon();
  }
}

/////////////////////////// //////////////// keyboard send
void kboardSend()
{
  unsigned int f = flag; flag = 10; String kSend = "keyboard send: \\ to enter a command - ESC to exit";
  if (f != 14) {
    refresh_Screen(); Serial.println(kSend);
  }
  else Serial.println("\n\r" + kSend);
  while (true) {
    getChars(true); if (keypress == '\r') Serial.println(); else if (keypress == ' ') delay(elementWait * 3);
    else if (keypress == 27) { // if ESC quit. go back to decoder
      clear_serial_buffer(); if (f == 14) Serial.println(F("\r\ndecoder - s to send")); return;
    }
    else if (keypress == '\\') { // char \  get command from decoder menu choices
      Serial.print(F("\n\rEnter command "));
      while (Serial.available() < 1); keypress = Serial.peek();
      if (keypress == 's') Serial.println(F("\n\rAlready in keyboard send mode"));
      else {
        getOption(); flag = 10; // skip routine if no keypress or not comimg from kboardSend()
      }
    }
    else
    {
      Serial.print(keypress);
      for (unsigned int i = 0; i < array; ++i) { // run through array to match char to send
        getStr(i); if (String(keypress) == buffer) { // match found
          getCode(i); send_code(buffer); break;
        }
      }
    }
  }
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
  for (tSend = 36; tSend < array; tSend++) { // punctuation
    sendChars();
  }
  send_code("-.-..-.");
  Serial.println(); clear_serial_buffer(); // Esc only to decode menu, not main menu
}

void sendChars()
{
  if (Serial.peek() == 27) return;
  getCode(tSend); send_code(buffer);
} // end keyboard send

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void decoder() // WB7FHC's Morse Code Decoder v.1.1 modified by BB.
{
  wpm_hold = wpm; flag = 0; decoder_stats(); decoder_menu();
  while (true)
  {
    getOption();
    if (flag == 1) {
      wpm = wpm_hold; eWait(); return;
    }

    // straight key routine // BB
    if (!digitalRead(P_dit)) {
      if (pitch != 0) tone(8, pitch); keyIsDown(); digitalWrite(signalPin, HIGH); if (relay == true) digitalWrite(relayPin, HIGH);
    }
    if (digitalRead(P_dit)) {
      noTone(8); keyIsUp(); digitalWrite(signalPin, LOW); digitalWrite(relayPin, LOW);
    } // BB

    //audio = digitalRead(audioPin); // What is the tone decoder doing? // original
    // audioPin is pulled high by a 470k resistor to 3.3v, for my mic module. If not pulled down by audio, Analog > threshold. // BB
    // My audioPin reads 695. Without the resistor (using this module) the reading is not stable!
    // I set threshold to audioPin - 10 or audioPin - 5 or audioPin - 40. 'a1' or 'a2' or 'a3'
    // So, high == no audio. Lower than threshold == audio.

    unsigned int Analog = analogRead(audioPin); if (Analog < threshold) audio = true; else audio = false;
    if (audio) keyIsDown();         // LOW, or 0, means tone is detected
    if (!audio) keyIsUp();          // HIGH, or 1, means no tone is there // BB
  }
}

void getOption() // BB
{
  if (Serial.peek() != -1 || keypress == '\\') // skip routine if no keypress and not coming from kboard_send()
  {
    flag = 9; // no screen prompt <
    getChars(true); // a key has been pressed
    switch (keypress)
    {
      case 'c': decoder_stats(); break;
      case 'a':
        if (value == 1) threshold = analogRead(audioPin) - 10;
        else if (value == 2) threshold = analogRead(audioPin) - 5;
        else if (value == 3) threshold = analogRead(audioPin) - 40;
        else if (value > 3) threshold = value;
        Serial.print(F("\n\raudioPin:")); Serial.print(analogRead(audioPin)); Serial.print(F(" - "));
        Serial.print(F("audio threshold:")); Serial.println(threshold);
        break;
      case 'r': Serial.print(F("\n\raudioPin:")); Serial.println(analogRead(audioPin)); break;
      case 'z': case ',': case '<': flag = 13; wpm = wpm - 5; setWPM(); break;
      case 'x': case '.': case '>': flag = 13; wpm = wpm + 5; setWPM(); break;
      case 'w': Serial.println(); if (value == 0)WPM(); else if (value == 1) hear_wpm(); else if (value > 7 && value < 101) {
          flag = 13; wpm = value; setWPM();
        }
        break;
      case 'v': err = 0; wpm_manual = !wpm_manual; Serial.print(F("auto wpm:"));
        if (!wpm_manual) Serial.print(F("on")); else Serial.print(F("off"));
        Serial.print(F(" w:")); Serial.println(wpm); break;
      case 'p': Serial.println(); setPitch(); break;
      case 't': send_all(); break;
      case 'k': if (value == 0) keyer_option(); break;
      case 'm': decoder_stats(); decoder_menu(); break;
      case 's': flag = 14; value = wpm; WPM(); kboardSend(); break;
      case 'd': code_chart(); break;
      case '\\': menu(); parser(); delay(3000); decoder_stats(); decoder_menu(); break;
      case  13: Serial.println(); break;
      case  27: flag = 1; break;
      default: break;
    }
    keypress = 0;
  }
}

void decoder_stats() // BB
{
  refresh_Screen();
  Serial.print(brag); Serial.print(F("[r:")); Serial.print(analogRead(audioPin)); Serial.print(F(" a:")); Serial.print(threshold);
  Serial.print(F(" p:")); Serial.print(pitch); Serial.print(F(" w:")); Serial.print(wpm); Serial.print(F(" v:"));
  if (wpm_manual) Serial.print(F("off")); else Serial.print(F("on"));
  Serial.print(F(" h:")); if (hear_speed) Serial.print(F("on")); else Serial.print(F("off")); Serial.print(F(" k:"));
  if (relay) Serial.print(F("on")); else Serial.print(F("off")); Serial.println(F("]"));
}

void decoder_menu() // BB
{
  Serial.println(F("\na (value) display/(set) audio threshold"));
  Serial.println(F("a1,2,3 adjust audio sensitivity: 1 default, 2 high, 3 low. a, just show level"));
  Serial.println(F("r display audioPin level"));
  Serial.println(F("w (value) display/(set) wpm (8-100) w1 toggle hear wpm sample"));
  Serial.println(F("z or < wpm - 5"));
  Serial.println(F("x or > wpm + 5"));
  Serial.println(F("v toggle auto wpm"));
  Serial.println(F("p set pitch"));
  Serial.println(F("t send all chars"));
  Serial.println(F("k toggle keyer relay"));
  Serial.println(F("s keyboard send"));
  Serial.println(F("d code chart"));
  Serial.println(F("\\ main menu command"));
}

void keyIsDown() {
  // The decoder is detecting our tone
  // The LEDs on the decoder and Arduino will blink on in unison
  // digitalWrite(13, 1);           // turn on Arduino's LED - not needed BB

  if (startUpTime > 0) {
    // We only need to do once, when the key first goes down
    startUpTime = 0;  // clear the 'Key Up' timer
  }
  // If we haven't already started our timer, do it now
  if (startDownTime == 0) {
    startDownTime = millis();  // get Arduino's current clock time
  }

  characterDone = false; // we're still building a character
  ditOrDah = false;      // the key is still down we're not done with the tone
  delay(myBounce);       // Take a short breath here

  if (myNum == 0) {      // myNum will equal zero at the beginning of a character
    myNum = 1;           // This is our start bit-it only does this once per letter
  }
}

void keyIsUp() {
  // The decoder does not detect our tone
  // The LEDs on the decoder and Arduino will blink off in unison
  // digitalWrite(13, 0);   // turn off Arduino's LED - not needed BB

  // If we haven't already started our timer, do it now
  if (startUpTime == 0) startUpTime = millis();

  // Find out how long we've gone with no tone
  // If it is twice as long as a dah print a space
  upTime = millis() - startUpTime;
  if (upTime < 10) return;
  if (upTime > averageDah * 2) printSpace();

  // Only do this once after the key goes up
  if (startDownTime > 0) {
    downTime = millis() - startDownTime;  // how long was the tone on?
    startDownTime = 0;    // clear the 'Key Down' timer
  }

  // We don't know if it was a dit or a dah yet. let's go find out! And do our Magic with the bits
  if (!ditOrDah) shiftBits();

  // If we are still building a character ...
  if (!characterDone) {
    // Are we done yet?
    if (upTime > dit) {
      // BINGO! we're done with this one
      printCharacter();     // Go figure out what character it was and print it
      characterDone = true; // We got him, we're done here
      myNum = 0;            // This sets us up for getting the next start bit
    }
    downTime = 0;           // Reset our keyDown counter
  }
}

void shiftBits() {
  // we know we've got a dit or a dah, let's find out which
  // then we will shift the bits in myNum and then add 1 or not add 1

  if (downTime < dit / 3) return;  // ignore my keybounce

  myNum = myNum << 1;   // shift bits left
  ditOrDah = true;      // we will know which one in two lines

  // If it is a dit we add 1. If it is a dah we do nothing!
  if (downTime < dit) myNum++;     // add one because it is a dit (really dit * 2)
  else if (wpm_manual == false) setWPM();
}

void setWPM() // The next three lines handle the automatic speed adjustment:
{
  if (wpm_manual || flag == 13) // BB
  {
    if (wpm < 8) wpm = 8; else if (wpm > 100) wpm = 100;
    averageDah = 3600 / wpm; eWait();
    clear_serial_buffer();////////////////////////////////////??????
    Serial.println("wpm:" + String(3600 / averageDah));
  }
  else
  {
    averageDah = (downTime + averageDah) / 2; // running average of dahs
    wpm = 3600 / averageDah;
    if (averageDah > 450) { // BB
      averageDah = 450; wpm = 8;
    }
    else if (averageDah < 36) { // BB
      averageDah = 36; wpm = 100;
    }
  }
  dit = averageDah / 3;    // normal dit would be this
  dit = dit * 2;           // double it to get the threshold between dits and dahs
  flag = 9;                // no screen prompt <
}

void printCharacter() {
  justDid = false;         // OK to print a space again after this
  if (myNum == 173 || myNum == 221) justDid = true; // cr-lf or clear_screen BB
  // Punctuation marks will make a BIG myNum
  if (myNum > 63) {
    printPunctuation();  // The value we parsed is bigger than our character array
    // It is probably a punctuation mark so go figure it out.
    return;              // Go back to the main loop(), we're done here.
  }
  printChar = mySet[myNum]; // Find the letter in the character set
  if (mySet[myNum] == '#') printPunctuation(); // not all punks are over 63 //BB-------------------------------------
  else sendToDisplay();     // print it
}

void printSpace() {
  if (justDid) return;  // only one space, no matter how long the gap
  justDid = true;       // so we don't do this twice

  // We keep track of the average gap between words and bump it up 20 milliseconds
  // to avoid false spaces within the word
  averageWordGap = ((averageWordGap + upTime) / 2) + 20;

  printChar = ' ';
  sendToDisplay();
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
    case 28: printChar = '^'; break;//..--
    case 41: printChar = '('; break;//-.--.
    case 53: printChar = '+'; break;//.-.-.
    case 55: printChar = '&'; break;//.-...
    case 71: printChar = ':'; break;//---...
    case 76: printChar = ','; break;//--..--
    case 82: printChar = ')'; break;//-...-
    case 84: printChar = '!'; break;//-.-.--
    case 85: printChar = ';'; break;//-.-.-.
    case 94: printChar = '-'; break;//-....-
    case 97: printChar = '\''; break; // Apostrophe .----.
    case 101: printChar = '@'; break;//.--.-.
    case 106: printChar = '.'; break;//.-.-.-
    case 109: printChar = '"'; break;//.-..-.
    case 114: printChar = '_'; break;//..--.-
    case 115: printChar = '?'; break;//..--..
    case 246: printChar = '$'; break;//...-..-
    case 122: printChar = 's'; sendToDisplay(); printChar = 'k'; break; // ...-.-
    case 173: Serial.println(); return; // -.-..-. bb new line (cr) return because, do not send to display
    case 221: decoder_stats(); return;  // .-...-. bb clear screen (lr)
    default:  printChar = '#'; // Serial.print(String(myNum)); //test to find new chars to add // BB
      startMaxSpeed(); break;  // if auto speed, start at max wpm // BB
  }
  sendToDisplay(); // go figure out where to put it on the display
}

void startMaxSpeed() // BB
{
  // This is to improve the auto speed which tends to hang on one long dah!
  // Auto works much better if it starts at max rather than min wpm.
  // This might not be the best place or best way to implement the idea but it's what I thought of.
  // Maybe there should be a time period to check for the number of errors. BB
  if (!wpm_manual) {
    if (printChar == '#') {
      err++; if (err > 3) { // for example
        err = 0; averageDah = 36; wpm = 100; eWait(); dit = averageDah / 3; dit = dit * 2;
      }
    }
  }
}

void sendToDisplay() {
  if (lower) printChar = toLowerCase(printChar); Serial.print(printChar); // BB
} //WB7FHC's Morse Code Decoder v.1.1 modified by BB.

/*
  const char mySet[] = "##TEMNAIOGKDWRUS##QZYCXBJP#L#FVH09#8###7#####/=61#######2###3#45";

  # # T E M N A I O G
  0 1 2 3 4 5 6 7 8 9

  K  D  W  R  U  S  #  #  Q  Z  Y  C  X  B  J  P  #  L  #  F  V  H  0  9  #  8  #
  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36

  #  #  7  #  #  #  #  #  /  =  6  1  #  #  #  #  #  #  #  2  #  #  #  3  #  4  5
  37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63

  a 6      0 32    ^ 28 ..--
  b 23     1 48    ( 41 -.--.
  c 21     2 56    + 53 .-.-.
  d 11     3 60    & 55 .-...
  e 3      4 62    : 71 ---...
  f 29     5 63    , 76 --..--
  g 9      6 47    ) 82 -.--.-
  h 31     7 39    ! 84 -.-.--
  i 7      8 35    ; 85 -.-.-.
  j 24     9 33    - 94 -....-
  k 10             ' 97 .---.
  l 27
  m 4              @ 101 .--.-.
  n 5              . 106 .-.-.-
  o 8              " 109 .-..-.
  p 25             _ 114 ..--.-
  q 18             ? 115 ..--..
  r 13             $ 246 ...-..-
  s 15
  t 2              sk 122 ...-.-
  u 14
  v 30
  w 12
  x 22
  y 20
  z 19
*/
