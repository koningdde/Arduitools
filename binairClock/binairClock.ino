#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds

//#define debug


const char *ssid     = "SSID";
const char *password = "PASSWORD";
/*
const char *ssid     = "AndroidAP2";
const char *password = "12345678";
*/

WiFiUDP ntpUDP;
// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


//Pin connected to ST_CP of 74HC595
int latchPin = 0;
//Pin connected to SH_CP of 74HC595
int clockPin = 4;
////Pin connected to DS of 74HC595
int dataPin = 5;

void setup(){
  pinMode(latchPin, OUTPUT);
  Serial.begin(115200);
  showOff();
  clearBin();
  WiFi.begin(ssid, password);
}

void loop() {

  while ( WiFi.status() != WL_CONNECTED ) {    
  clearBin();
  reConnect();
  }
  
  currentMillis = millis(); 
  if (currentMillis - startMillis >= period){
  timeClient.update();
  startMillis = currentMillis;
  displayTime();
  }
  
  prepareTime();

}


void shiftOut(int myDataPin, int myClockPin, long myDataOut) {
  // This shifts 32 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

 //clear everything out just in case to
 //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=32; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}


void displayTime(){
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
}

void prepareTime(){
  int hours = (timeClient.getHours());
  int onesHours = (hours%10);
  int tensHours = ((hours/10)%10) << 4;
  int hoursOut = tensHours | onesHours;

  #ifdef debug
  Serial.println(hoursOut,BIN);
  #endif
  
  int mins = (timeClient.getMinutes());
  int onesMins = (mins%10);
  int tensMins = ((mins/10)%10) << 4;
  int minsOut = tensMins | onesMins;
  int set1 = (hoursOut << 8) | minsOut;
  
  #ifdef debug
  Serial.println(minsOut,BIN);
  Serial.println(set1,BIN);
  #endif
    
  int sec = (timeClient.getSeconds());
  int onesSec = (sec%10);
  int tensSec = ((sec/10)%10) << 4;
  int secOut = tensSec | onesSec;
  long set2 = (set1 << 8) | secOut;
  #ifdef debug
  Serial.println(secOut,BIN);
  Serial.println(set2,BIN);
  #endif

  if (sec > 3){
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, set2);
  digitalWrite(latchPin, 1);
  }
  else{
    showTemp();
  }
}

void reConnect(){
   while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  startMillis = millis();  //initial start time
}

void clearBin(){
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0);
  digitalWrite(latchPin, 1);
}

void showOff(){
  for(int x=0; x<4; x++){
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0b111111110000000000000000);
  digitalWrite(latchPin, 1);
  delay(200);
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0b000000001111111100000000);
  digitalWrite(latchPin, 1);
  delay(200);
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0b000000000000000011111111);
  digitalWrite(latchPin, 1);
  delay(200);
  }
}

void showTemp(){
  int temp = 21;
  int degC = 0b01110101;
  long neg;
  if (temp < 0){
    neg = 0b00100010 << 16;
  }
  else{
    neg = 0b00000000 << 16;
  }
  temp = abs(temp);
  long tempLSB = (temp%10) << 8;
  long tempMSB = ((temp/10)%10) << 12;
  long tempOut = neg | tempMSB | tempLSB | degC;
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, tempOut);
  digitalWrite(latchPin, 1);
}
