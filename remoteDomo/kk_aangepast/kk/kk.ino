#include "kk.h"

int buttonon = 7;
int buttonoff = 6;

void setup() { 
  Serial.begin(9600); 
  Serial.println("start");
  kk_init(); 
  delay(1500);
  kk_send(12509470,0,1,0);
  }

char * parse(char **s) { char *p = *s; char *q; while (*p==' ') p++; q = p; while ((*p!=' ') && (*p!=0)) p++; if (*p==' ') *p++ = 0; *s = p; return q; }

void loop() { 
  digitalWrite(buttonon, INPUT_PULLUP);
  digitalWrite(buttonoff, INPUT_PULLUP);
  
  unsigned long address; 
  unsigned char unit,dimlevel,onoff; 
  static char inbuf[20]; 
  static unsigned char i=0; 
  char *p,*s,c; 
  
  
  if (kk_available()) { 
    kk_receive(&address,&unit,&onoff,&dimlevel); 
    Serial.print("A: "); 
    Serial.print(address); 
    Serial.print("U: "); 
    Serial.print(unit); 
    Serial.print("C: "); 
    Serial.print((onoff==0)?"Off":(onoff==1)?"On ":"Dim"); 
    Serial.print("D: "); 
    Serial.println(dimlevel); 
} 
//kk_send(12509470,0,1,0);

  if(digitalRead(buttonon) == LOW){
    
    for(i=0; i < 3; i++) {
    kk_send(0,0,1,0);
    Serial.print("ON");
    delay(100);}
  }
  
  if(digitalRead(buttonoff) == LOW){
    for(i=0; i<3; i++){
    kk_send(0,0,0,0);
    delay(100);
    Serial.print("OFF");
    }   
  }

if ((i<19) && (c!=13)) { 
  inbuf[i++] = c; 
  }

}//End main loop


