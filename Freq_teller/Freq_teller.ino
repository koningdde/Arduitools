//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
const int pulsePin = 12; // Input signal connected to Pin 12 of Arduino
int pulseHigh; // Integer variable to capture High time of the incoming pulse
int pulseLow; // Integer variable to capture Low time of the incoming pulse
float pulseTotal; // Float variable to capture Total time of the incoming pulse
float frequency; // Calculated Frequency
float duty;

void setup()
{
  pinMode(pulsePin, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Instructables");
  lcd.setCursor(0, 1);
  lcd.print(" Freq Counter ");
  delay(5000);
  lcd.clear();
}


void loop()
{
pulseHigh = pulseIn(pulsePin, HIGH);
pulseLow = pulseIn(pulsePin, LOW);
pulseTotal = pulseHigh + pulseLow; // Time period of the pulse in microseconds
frequency = 1000000 / pulseTotal; // Frequency in Hertz (Hz)
duty = pulseHigh/pulseTotal;
lcd.setCursor(0, 0);
lcd.print("Freq");
lcd.setCursor(6, 0);
lcd.print(frequency);
lcd.print(" Hz      ");
lcd.setCursor(0, 1);
lcd.print("Cycle ");
lcd.print(duty);
lcd.print(" %");
delay(500);

}

