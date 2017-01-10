/*
 * PIR sensor tester
 */
 
int green = 13;// choose the pin for the LED
int red = 11;
int blue = 12;

int inputPin = A3;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
 
void setup() {
  pinMode(blue, OUTPUT);      // declare LED as output
  pinMode(green, OUTPUT);      // declare LED as output
  pinMode(red, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
  digitalWrite(A3, HIGH);
  Serial.begin(9600);
  digitalWrite(blue, HIGH);
  delay(500);
  digitalWrite(blue, LOW);
  digitalWrite(red, HIGH);
  delay(500);
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  delay(500);
  digitalWrite(green, LOW);
}
 
void loop(){
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(blue, HIGH);  // turn LED ON
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    digitalWrite(blue, LOW); // turn LED OFF
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}
