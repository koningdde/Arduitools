//TBV Klikaan
#include "kk.h"
char * parse(char **s) { char *p = *s; char *q; while (*p==' ') p++; q = p; while ((*p!=' ') && (*p!=0)) p++; if (*p==' ') *p++ = 0; *s = p; return q; }
unsigned long address; 
unsigned char unit,dimlevel,onoff; 
static char inbuf[20]; 
static unsigned char i=0; 
char *p,*s,c; 


void kkOut(char adres, char state){
    switch (adres) {
    case '0':  
      switch (state){
        case '0':
            for(i=0; i < 3; i++) {
            kk_send(0,0,0,0);
            Serial.print("OFF");
            delay(100);}
        break;
        case '9':
            for(i=0; i < 3; i++) {
            kk_send(0,0,1,0);
            Serial.print("ON");
            delay(100);}
        break;
      }
    }
}
