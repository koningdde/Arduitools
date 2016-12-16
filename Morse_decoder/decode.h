
int audioPin = decoderpin;// we read data from the tone detector module here
int audio = 1;            // will store the value we read on this pin
int LCDline = 1;          // keeps track of which line we're printing on
int lineEnd = 21;         // One more than number of characters across display
int letterCount = 0;      // keeps track of how may characters were printed on the line
int lastWordCount = 0;    // keeps track of how may characters are in the current word
int lastSpace = 0;        // keeps track of the location of the last 'space'

// The next line stores the text that we are currently printing on a line,
// The charcters in the current word,
// Our top line of text,
// Our second line of text,
// and our third line of text
// For a 20x4 display these are all 20 characters long
char currentLine[] = "12345678901234567890";
char    lastWord[] = "                    ";
char       line1[] = "                    ";
char       line2[] = "                    ";
char       line3[] = "                    ";

boolean ditOrDah = true;  // We have either a full dit or a full dah
int dit = 10;             // We start by defining a dit as 10 milliseconds

// The following values will auto adjust to the sender's speed
int averageDah = 240;             // A dah should be 3 times as long as a dit
int averageWordGap = averageDah;  // will auto adjust
long fullWait = 6000;             // The time between letters
long waitWait = 6000;             // The time between dits and dahs
long newWord = 0;                 // The time between words

boolean characterDone = true; // A full character has been sent

int downTime = 0;        // How long the tone was on in milliseconds
int upTime = 0;          // How long the tone was off in milliseconds
int myBounce = 2;        // Used as a short delay between key up and down

long startDownTime = 0;  // Arduino's internal timer when tone first comes on
long startUpTime = 0;    // Arduino's internal timer when tone first goes off

long lastDahTime = 0;    // Length of last dah in milliseconds
long lastDitTime = 0;    // Length oflast dit in milliseconds
long averageDahTime = 0; // Sloppy Average of length of dahs

boolean justDid = true; // Makes sure we only print one space during long gaps

int myNum = 0;           // We will turn dits and dahs into a binary number stored here

/////////////////////////////////////////////////////////////////////////////////
// Now here is the 'Secret Sauce'
// The Morse Code is embedded into the binary version of the numbers from 2 - 63
// The place a letter appears here matches myNum that we parsed out of the code
// #'s are miscopied characters
char mySet[] ="##TEMNAIOGKDWRUS##QZYCXBJP#L#FVH09#8###7#####/-61#######2###3#45";
char lcdGuy = ' ';       // We will store the actual character decoded here

/////////////////////////////////////////////////////////////////////////////////



