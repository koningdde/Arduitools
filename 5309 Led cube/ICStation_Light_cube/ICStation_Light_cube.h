/************************************************************************************
  All materials are provided by www.ICStation.com.
Should you have any unclear or need any other related material, please feel free to 
contact us via http://www.icstation.com/contact_us.php.
www.ICStation.com
   ICStation is based in Shenzhen, China, a city which is rich in electronics. 
With such manufacture power, we not only provide worldwide with all kinds of 
IC products, such as electronic devices and components, development modules, 
development boards, consumptive materials and so on, but also we are taking 
part in the designing and developing digital and analog circuit design which
 based on  microcontroller platforms.
**************************************************************************************/

#ifndef ICSTATION_LIGHT_CUBE_H
#define ICSTATION_LIGHT_CUBE_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#ifndef _BV
#define _BV
#define _BV(a)   (0x0001 << (a))
#endif





class ICStation_Light_cube {

 public:

  // Constructor: number of LEDs, pin number, LED type
  ICStation_Light_cube();
  ~ICStation_Light_cube();
void my_display(unsigned char *planeBuf);
 void dight_write_LED_pin( int value,int charge);  //write data to 74HC5595
 void run_example(void);
 
 
 
 private:
static int LEDPin[];
static int PlanePin[4];
static unsigned char PatternTable[];
static int clockPin;  //SH_CP of 74HC595 
static int latchPin;  //ST_CP of 74HC595 
static int dataPin;  //DS of 74HC595 
static int HC_595_E;  //74HC595 PIN E
static int LED_Pin16;
static int LED_Pin17;
static int LED_Pin18;
static int LED_Pin19;  
static unsigned int HC_595_Temp;   //74HC595write_temp
static int  ICStation_delay;
static int PatternIdx ;// indexes which byte from pattern buffer
static int PatternMax;



void write_74HC595(void);  //write data to 74HC5595
};

#endif // ADAFRUIT_NEOPIXEL_H
