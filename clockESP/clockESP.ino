#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

const char * ssid = "ZuluHotelEcho"; // your network SSID (name)
const char * password = "PapaIndia";  // your network password

unsigned long previousMillis = 0; 
const long interval = 800;
int hours;
int minutes;
int sec;
    
#include <FastLED.h>

//#define LED_PIN     1
#define NUM_LEDS 60
#define DATA_PIN 1
CRGB leds[NUM_LEDS];

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


void setup(){
  FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);

    // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
}

void loop() {
 //get a random server from the pool
  unsigned long currentMillis = millis(); 

  if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        timeClient.update();

          Serial.println(timeClient.getFormattedTime());
 
    hours = (timeClient.getHours());
    minutes = (timeClient.getMinutes());
    sec = (timeClient.getSeconds());
    hours = hours + 1;

    int hourspart = minutes/12;
  
        if(hours > 12){
          hours = hours - 12;
        }
    hours = (hours*5) + hourspart;
    }

  //Writing leds   
  for (int i = 0; i<60; i++){
    leds[i].setRGB(0,0,0);
    }

   
    leds[sec].setRGB(100,0,0);
    //leds[sec-1].setRGB(25,0,0);
    //leds[sec-2].setRGB(6,0,0);
    leds[minutes].setRGB(0,100,0);
    leds[hours].setRGB(0,0,100);
    //leds[hours-1].setRGB(0,0,100);

    FastLED.show();

  delay(200);
  }

 
