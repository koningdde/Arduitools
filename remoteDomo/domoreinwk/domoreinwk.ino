// Library's
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
Adafruit_BMP085 bmp;
#define clientId "tuinkamer"
#include "config.h"

// Network settings
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xCC };
IPAddress ip(192, 168, 100, 202);
IPAddress server(192, 168, 100, 54);

//Variables
char unitId = '2'; //Unit id
int idx1 = 13; //IDX number for domoticz
int idx2 = 60;
int idx3 = 61;
int idx4 = 61;
float data1 = 0.0; //Datapoint
float data2 = 0;
float data3 = 0.0;
float data4 = 0.0;


//Holders
int coReset;
int waterReset;
int koudReset;
int postReset;
char rpiIp[20];

unsigned long last;
unsigned long interval = (6000); //Interval to send sensor data

EthernetClient ethClient;
PubSubClient client(ethClient);
      
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);      
  }
  Serial.println();
  if ((char)payload[0] == unitId){
   
    switch ((char)payload[1]){
      case 'r':
       relayOut( (char)payload[2],(char)payload[3] );
       break;
       case 't':
       lcd.setCursor(0,3);
       lcd.write( (char)payload[4]);
       lcd.write( (char)payload[5]);
       lcd.write(":");
       lcd.write( (char)payload[6]);
       lcd.write( (char)payload[7]);
       lcd.setCursor(10,3);
       lcd.write( (char)payload[10]);
       lcd.write( (char)payload[11]);
       lcd.write("-");
       lcd.write( (char)payload[8]);
       lcd.write( (char)payload[9]);
       lcd.write("-");
       lcd.write( (char)payload[12]);
       lcd.write( (char)payload[13]); 
       lcd.write( (char)payload[14]);
       lcd.write( (char)payload[15]);         
       break;
       case 'i':
       for (int i = 2; i < 20; i++) { // store rpi IP adres
       rpiIp[i]= ((char)payload[i]);}
       break;
    }//end switch case
    }//enf first iff
  
  else {
      Serial.println("reject unitId");
      }
}

void relayOut(char relay, char state){  

  switch (relay) {
    case '0':  
      switch (state){
        case '0':
        digitalWrite(relay1, LOW);
        break;
        case '9':
        digitalWrite(relay1, HIGH);
        break;
      }
    break;   
    case '1':  
      switch (state){
        case '0':
        digitalWrite(relay2, LOW);
        break;
        case '9':
        digitalWrite(relay2, HIGH);
        break;
      }
    break;
    case '2':  
      switch (state){
        case '0':
        digitalWrite(relay3, LOW);
        break;
        case '9':
        digitalWrite(relay3, HIGH);
        break;
      }
    break;
    case '3':  
      switch (state){
        case '0':
        digitalWrite(relay4, LOW);
        break;
        case '9':
        digitalWrite(relay4, HIGH);
        break;
      }
    break;   
  }
}
void sensorDataout(int idx, float data){
      Serial.println("Sending data....");
      String string = " { \"idx\" : ";
      string.concat(idx);
      string.concat(", \"nvalue\" : 0, \"svalue\" : \"");
      string.concat(data);
      string.concat("\" \}");
      Serial.println(string);
      char output[80];
      string.toCharArray(output, 80);
      //char data2[] ={"\{\"command\": \"switchlight\", \"idx\": 14, \"switchcmd\": \"Off\", \"level\": 100\}"};
      client.publish("domoticz/in",output);
      // ... and resubscribe
      client.subscribe("domoticz/arduino");

  
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
      lcd.setCursor(0,0);
      lcd.print("Connected           ");
      client.subscribe("domoticz/arduino");       
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  digitalWrite(buzzer, HIGH); //BUZZER OFF
  lcd.begin(20, 4);
  lcd.print("Systeem starten");
  lcd.setCursor(0,1);
  lcd.print("Wachten aub");
  lcd.setCursor(0,2);
  lcd.print("Rein Oud");
  lcd.setCursor(0,3);
  lcd.print("Domoticz systeem");
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  
//Pin setup
  pinMode (ledKoud, OUTPUT); 
  pinMode (ledKas, OUTPUT); 
  pinMode (ledPost, OUTPUT); 
  pinMode (ledCo, OUTPUT); 
  pinMode (ledBodem, OUTPUT); 
  pinMode (ledWater, OUTPUT); 
  pinMode (buzzer, OUTPUT); 
  pinMode (butCo, INPUT);
  pinMode (butWater, INPUT);
  pinMode (butKoud, INPUT);
  pinMode (butBrief, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  
  ledtest(); //Preform ledtest
  
  Serial.begin(57600);
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
  last = millis();
  lcd.clear();
}

void loop()
{
  if (!client.connected()) {//Reconnect not connected
    lcd.setCursor(0,0);
    lcd.print("Not connected       ");
    reconnect();
  }
  client.loop();
  if ((millis() - last) >= interval){ //To start sending sensordata
    data1 = bmp.readTemperature();
    data2 = ((bmp.readPressure()/100));
    lcd.setCursor(0,1);
    lcd.print(data2);
    lcd.print(" mBar        ");
    //sensorDataout(idx1, data1);
    //sensorDataout(idx2, data2);
    //sensorDataout(idx3, data3);
    //sensorDataout(idx4, data4);
    last = millis(); 
  }

buttonRead();


}//end main loop

void ledtest() 
  {
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  digitalWrite(ledKoud, HIGH);
  digitalWrite(ledKas, HIGH);
  digitalWrite(ledPost, HIGH);
  digitalWrite(ledCo, HIGH);
  digitalWrite(ledBodem, HIGH);
  digitalWrite(ledWater, HIGH);
  delay(2000);
  digitalWrite(ledKoud, LOW);
  digitalWrite(ledKas, LOW);
  digitalWrite(ledPost, LOW);
  digitalWrite(ledCo, LOW);
  digitalWrite(ledBodem, LOW);
  digitalWrite(ledWater, LOW);
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
  }

void buttonRead()
{
coReset = digitalRead(butCo);
if (coReset == LOW) { digitalWrite(ledCo, HIGH); }
if (coReset == HIGH) { digitalWrite(ledCo, LOW); }

waterReset = digitalRead(butWater);
if (waterReset == LOW) { digitalWrite(ledWater, HIGH); }
if (waterReset == HIGH) { digitalWrite(ledWater, LOW); }

koudReset = digitalRead(butKoud);
if (koudReset == LOW) { digitalWrite(ledKoud, HIGH); }
if (koudReset == HIGH) { digitalWrite(ledKoud, LOW); }

postReset = digitalRead(butBrief);
if (postReset == LOW) { digitalWrite(ledPost, HIGH); }
if (postReset == HIGH) { digitalWrite(ledPost, LOW); }

if ((postReset == LOW) && (koudReset == LOW)) {ledtest();}
if ((coReset == LOW) && (waterReset == LOW)) {showIp();}
}

void showIp(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("IP: ");
      lcd.print(ip);
      lcd.setCursor(0,1);
      lcd.print("RPI: ");
      for (int i = 2; i < 16; i++) {
      lcd.print(rpiIp[i]); }
      delay(2000);
      lcd.setCursor(0,0);
      lcd.print("Connected           ");
}

