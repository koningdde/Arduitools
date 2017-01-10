#include <IRLib.h>
#define MY_PROTOCOL NEC
#define BUTTON_0 0x20DF40BF
#define BUTTON_1 0x20DFC03F

IRrecv My_Receiver(9);//Receive on pin 11
IRdecode My_Decoder; 


void setup() {
  // put your setup code here, to run once:
  My_Receiver.No_Output();//Turn off any unused IR LED output circuit
  My_Receiver.enableIRIn(); // Start the receiver
  pinMode(13, OUTPUT); 
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
if (My_Receiver.GetResults(&My_Decoder)) {
       My_Decoder.decode();
       Serial.print("decode");
       if(My_Decoder.decode_type==MY_PROTOCOL) {
          switch(My_Decoder.value) {
          case BUTTON_0:    digitalWrite(13, HIGH); break;
          case BUTTON_1:    digitalWrite(13, LOW); break;
          }
}
delay(100);
digitalWrite(13, LOW);

}
}
