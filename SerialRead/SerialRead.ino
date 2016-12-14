#include "config.h"

String info;


// while(Serial.available()>0) Serial.read(); //IPV flush

void setup()
{
Serial.begin(baud);
}

void loop()
{
    while(Serial.available()>0)  //wait for empty buffer
    {    
        info = Serial.readString(); 
    }
 
 
 if (info.charAt(1) == 'B') {
    Serial.println(info);
 }

if (info.startsWith("!a")) {
     Serial.println(info); 
}

} //end of program

