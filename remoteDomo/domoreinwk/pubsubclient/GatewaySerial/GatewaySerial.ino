/**
* The MySensors Arduino library handles the wireless radio link and protocol
* between your home built sensors/actuators and HA controller of choice.
* The sensors forms a self healing radio network with optional repeaters. Each
* repeater and gateway builds a routing tables in EEPROM which keeps track of the
* network topology allowing messages to be routed to nodes.
*
* Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
* Copyright (C) 2013-2015 Sensnology AB
* Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
*
* Documentation: http://www.mysensors.org
* Support Forum: http://forum.mysensors.org
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*
*******************************
*
* DESCRIPTION
* The ArduinoGateway prints data received from sensors on the serial link.
* The gateway accepts input on seral which will be sent out on radio network.
*
* The GW code is designed for Arduino Nano 328p / 16MHz
*
* Wire connections (OPTIONAL):
* - Inclusion button should be connected between digital pin 3 and GND
* - RX/TX/ERR leds need to be connected between +5V (anode) and digital pin 6/5/4 with resistor 270-330R in a series
*
* LEDs (OPTIONAL):
* - To use the feature, uncomment any of the MY_DEFAULT_xx_LED_PINs
* - RX (green) - blink fast on radio message recieved. In inclusion mode will blink fast only on presentation recieved
* - TX (yellow) - blink fast on radio message transmitted. In inclusion mode will blink slowly
* - ERR (red) - fast blink on error during transmission error or recieve crc error
*
*/

// Enable debug prints to serial monitor
#define MY_DEBUG


// Enable and select radio type attached
//#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
#define MY_RF24_PA_LEVEL RF24_PA_LOW

// Enable serial gateway
#define MY_GATEWAY_SERIAL

// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

// Enable inclusion mode
#define MY_INCLUSION_MODE_FEATURE
// Enable Inclusion mode button on gateway
//#define MY_INCLUSION_BUTTON_FEATURE

// Inverses behavior of inclusion button (if using external pullup)
//#define MY_INCLUSION_BUTTON_EXTERNAL_PULLUP

// Set inclusion mode duration (in seconds)
#define MY_INCLUSION_MODE_DURATION 60
// Digital pin used for inclusion mode button
//#define MY_INCLUSION_MODE_BUTTON_PIN  3

// Set blinking period
#define MY_DEFAULT_LED_BLINK_PERIOD 300

// Inverses the behavior of leds
//#define MY_WITH_LEDS_BLINKING_INVERSE

// Flash leds on rx/tx/err
// Uncomment to override default HW configurations
//#define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
//#define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
//#define MY_DEFAULT_TX_LED_PIN  5  // the PCB, on board LED

const unsigned long tUpdate=60000; //update interval
unsigned long t0;

#include <MySensors.h>
#define CHILD_ID 1   // Id of the sensor child
#define CHILD_ID2 2   // Id of the sensor child
#define CHILD_ID3 3   // Id of the sensor child
MyMessage msgPir(CHILD_ID, V_TRIPPED); // 16
#define PIR_PIN 2
byte StatePIR=0;
byte oldStatePIR=0;
byte StateREL=0, StateREL1=0, StateREL2=0;

#define REL1_PIN 7
#define REL2_PIN 8
MyMessage msgREL1(CHILD_ID2, V_LIGHT);  // 2
MyMessage msgREL2(CHILD_ID3, V_LIGHT);  // 2

void setup()
{
	// Setup locally attached sensors
  pinMode(PIR_PIN, INPUT_PULLUP);
  pinMode(REL1_PIN, OUTPUT);
  digitalWrite(REL1_PIN, StateREL1);
  pinMode(REL2_PIN, OUTPUT);
  digitalWrite(REL2_PIN, StateREL2);

  t0=millis();
}

void presentation()
{
	// Present locally attached sensors
 // Send the sketch version information to the gateway and Controller
 sendSketchInfo("Motion Sensor", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID, S_MOTION);
  present(CHILD_ID2, S_LIGHT);
  present(CHILD_ID3, S_LIGHT);
}

void loop()
{
	// Send locally attached sensor data here
 StatePIR=digitalRead(PIR_PIN);
  if (StatePIR != oldStatePIR) {
    oldStatePIR=StatePIR;
    ServerUpdate();
  }
  if ((millis()-t0) > tUpdate) ServerUpdate();
}



void ServerUpdate() {
  gw.present(1,1);
  send(msgPir.set(StatePIR ? "ON" : "OFF"));
  t0=millis();
}

void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_LIGHT) {
    StateREL=strcmp(message.data,"ON")==0;
    switch(message.sensor) {
      case 101: {
        StateREL1=StateREL;
        if (StateREL) {
          digitalWrite(REL1_PIN, HIGH);
        }
        else {
          digitalWrite(REL1_PIN, LOW);
        }
        send(msgREL1.set(StateREL1 ? "ON":"OFF"));
      }
      break;
      case 102: {
        StateREL2=StateREL;
        if (StateREL) {
          digitalWrite(REL2_PIN, HIGH);
        }
        else {
          digitalWrite(REL2_PIN, LOW);
        }
        send(msgREL2.set(StateREL2 ? "ON":"OFF"));
      }
      break;
    }

   }
} 


