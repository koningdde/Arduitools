
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 9600;
unsigned long last = 0;
unsigned long interval = 300000;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
float latitude = 0.0;
float longitude = 0.0;


void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  
  pinMode(9, OUTPUT);
  pinMode(3, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
   
  while (ss.available() > 0)
    gps.encode(ss.read());

    
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

displayInfo();

latitude = gps.location.lat();
longitude = gps.location.lng();
//latitude = 51.902582;
//qlongitude = 4.163685;

if ((millis()-last) > interval){
  char buf[10];
  dtostrf(latitude, 2, 6, buf);

  char buf2[10];
  dtostrf(longitude, 1, 6, buf2);

  Serial.println("START");
  digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500); 

  for (int x = 0; x < 10; x++) {
    Serial.println(buf[x]);
    senddata(buf[x]);
    }
  
  delay(2500); 
  
  for (int x = 0; x < 10; x++) {
    Serial.println(buf2[x]);
    senddata(buf2[x]);
    }
  digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
  last = millis();
  }
}

void senddata(char buf3)
{
  //Serial.println(buf3);
  int i = 0;
  switch (buf3) {
    case '1':
      i = 1;
      sendout(i);
      break;
    case '2':
      i = 2;
      sendout(i);
      break;
    case '3':
      i = 3;
      sendout(i);
      break;
    case '4':
      i = 4;
      sendout(i);
      break;
    case '5':
      i = 5;
      sendout(i);
      break;
    case '6':
      i = 6;
      sendout(i);
      break;
    case '7':
      i = 7;
      sendout(i);
      break;
    case '8':
      i = 8;
      sendout(i);
      break;
    case '9':
      i = 9;
      sendout(i);
      break;
    case '0':
      i = 10;
      sendout(i);
      break;
        
    default:
      // if nothing else matches, do the default
      // default is optional
    break;
    
  }
}

void sendout(int i){
  //Serial.print(i);
  for (int x = 0; x < i; x++) {
   
   tone(3, (500), 200);
   delay(1000);
   //stop the tone playing:
   noTone(3);
  }
  delay(1000);
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

