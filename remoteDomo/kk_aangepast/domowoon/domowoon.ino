#include <IRLib.h>
#define MY_PROTOCOL NEC
#define BUTTON_0 0x20DF40BF
#define BUTTON_1 0x20DFC03F

IRrecv My_Receiver(3);//Receive on pin 3
IRdecode My_Decoder; 
const byte interruptPin = 3;


int green = 13; // Led out
int red = 11;   // Led out
int blue = 12;  // Led out

int pir = A3;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0; 

void setup() {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), irdetect, CHANGE); //Interrupt basis, lees IRcode

  My_Receiver.No_Output();//Turn off any unused IR LED output circuit
  My_Receiver.enableIRIn(); // Start the receiver

  pinMode(blue, OUTPUT);      // declare LED as output
  pinMode(green, OUTPUT);      // declare LED as output
  pinMode(pir, INPUT);     // declare sensor as input
  pinMode(red, OUTPUT);      // declare LED as output
  Serial.begin(9600);
}//end setup

void loop() {

  val = digitalRead(pir);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(red, HIGH);  // turn LED ON
    
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    digitalWrite(red, LOW); // turn LED OFF
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
} //end main loop


void irdetect(){if (My_Receiver.GetResults(&My_Decoder)) {
       My_Receiver.resume();
       My_Decoder.decode();
       digitalWrite(green, HIGH);
       Serial.println("decode");
       if(My_Decoder.decode_type==MY_PROTOCOL) {
          switch(My_Decoder.value) {
          case BUTTON_0:    Serial.print("aan"); break;
          case BUTTON_1:    Serial.print("uit"); break;
          }

      }
      digitalWrite(green, LOW);
} //end irdetect
} //end of program

