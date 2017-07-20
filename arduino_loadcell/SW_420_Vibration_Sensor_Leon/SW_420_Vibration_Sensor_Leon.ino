/* Demo Programma vibratie unit Leon
   By RvG (C)2017 */
int LED_Pin = 13;

int vibr_Pin =3;

void setup(){
  pinMode(LED_Pin, OUTPUT);
  pinMode(vibr_Pin, INPUT); //set vibr_Pin input for measurment
  Serial.begin(9600); //init serial 9600
 // Serial.println("----------------------Vibration demo------------------------");
}
void loop(){
  long measurement =TP_init();
  delay(50);
 // Serial.print("measurment = ");
  Serial.println(measurement);
  if (measurement > 100000){digitalWrite(LED_Pin, LOW); //drempelwaarde impact
    delay(1000);
  Serial.print("HIT = ");}  
  else{
    digitalWrite(LED_Pin, HIGH); 
  }
}

long TP_init(){
  delay(10);
  long measurement=pulseIn (vibr_Pin, HIGH);  //wait for the pin to get HIGH and returns measurement
  return measurement;
}

