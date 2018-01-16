/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h> 
OneWire ds(14); //Dallas op pin

const char* ssid = "Jupiter";
const char* password = "3827310955370393";
const char* mqtt_server = "192.168.100.54";

#define clientId "ESPGarage"
char unitId = '7'; //Unit id
int idx1 = 258; //IDX number for domoticz
float data1 = 0.0; //Datapoint
float data2 = 0;

int rood = 13; //Hardwire output
int groen = 15; //Hardwire output

int garageDeur = 16;
int deelIn = 2;
int volIn = 4;
int meterkast = 5;
int entreedeur = 12;
int garage = 10;

int val = 0;
int val2 = 0;
int val3 = 0;
int val4 = 0;
int val5 = 0;
int val6 = 0;
bool state;
bool state2;
bool state3;
bool state4;
bool state5;
bool state6;
bool alarmOnVol;
bool alarmOnDeel;

unsigned long last;
unsigned long interval = 300000; //Interval to send sensor data
//unsigned long interval = 3000; //Interval to send sensor data
const long blinkInterval = 500;
long blinkLast = 0;
bool blinker;
bool ledState;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(rood, OUTPUT);
  pinMode(groen, OUTPUT);
  pinMode(garageDeur, INPUT_PULLUP);     // declare sensor as input
  pinMode(deelIn, INPUT_PULLUP);     // declare sensor as input
  pinMode(volIn, INPUT_PULLUP);     // declare sensor as input
  pinMode(meterkast, INPUT_PULLUP);     // declare sensor as input
  pinMode(entreedeur, INPUT_PULLUP);     // declare sensor as input
  pinMode(garage, INPUT_PULLUP);     // declare sensor as input
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
 
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

   if ((millis() - last) >= interval){ //To start sending sensordata
    data1 = dallas();
    data2 = 0.0;
    sensorDataout(idx1, data1, data2);
    last = millis();   
  }

  //Read connected sensors
val = digitalRead(garageDeur);  // read input value
val2 = digitalRead(deelIn);  // read input value
val3 = digitalRead(volIn);  // read input value
val4 = digitalRead(meterkast);  // read input value
val5 = digitalRead(entreedeur);  // read input value
val6 = digitalRead(garage);  // read input value

if (val == HIGH && state == LOW)
      {
      lightOut(218,"Off"); //idx 26, uit, sturen naar domoticz
      Serial.println("Garage dicht");
      state = HIGH;
      }

if (val == LOW && state == HIGH)
    {   
        lightOut(218,"On"); //idx 26, uit, sturen naar domoticz
        Serial.println("Garegadeur open");
        state = LOW;
    }

if (val2 == HIGH && state2 == LOW)
      {
      lightOut(278,"Off"); //idx 26, uit, sturen naar domoticz
      Serial.println("Deel uit");
      state2 = HIGH;
      }

if (val2 == LOW && state2 == HIGH && alarmOnVol == false && alarmOnDeel == false)
    {   
        lightOut(278,"On"); //idx 26, uit, sturen naar domoticz
        Serial.println("Deel in");
        state2 = LOW;
     }

if (val3 == HIGH && state3 == LOW)
      {
      lightOut(277,"Off"); //idx 26, uit, sturen naar domoticz
      Serial.println("Vol uit");
      state3 = HIGH;
      }

if (val3 == LOW && state3 == HIGH)
    {   
        lightOut(277,"On"); //idx 26, uit, sturen naar domoticz
        Serial.println("Vol in");
        state3 = LOW;
        blinker = true;
    }

if (val4 == HIGH && state4 == LOW)
      {
      lightOut(280,"On"); //idx 26, uit, sturen naar domoticz
      Serial.println("Meterkast open");
      state4 = HIGH;
      }

if (val4 == LOW && state4 == HIGH)
    {   
        lightOut(280,"Off"); //idx 26, uit, sturen naar domoticz
        Serial.println("Meterkast dicht");
        state4 = LOW;
    }

if (val5 == HIGH && state5 == LOW)
      {
      lightOut(336,"On"); //idx 26, uit, sturen naar domoticz
      Serial.println("Entreedeur open");
      state5 = HIGH;
      }

if (val5 == LOW && state5 == HIGH)
    {   
        lightOut(336,"Off"); //idx 26, uit, sturen naar domoticz
        Serial.println("Entreedeur dicht");
        state5 = LOW;
    }

if (val6 == HIGH && state6 == LOW)
      {
      lightOut(337,"On"); //idx 26, uit, sturen naar domoticz
      Serial.println("Garage beweging aan");
      state6 = HIGH;
      }
      
if (val6 == LOW && state6 == HIGH)
    {   
        lightOut(337,"Off"); //idx 26, uit, sturen naar domoticz
        Serial.println("Garage beweging uit");
        state6 = LOW;
    }

if ((blinker == true) && (millis() - blinkLast >= blinkInterval))
    {
    blinkLast = millis();

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(rood, ledState);
    }
     
}//end main loop

int dallas(){
  byte data[2];
  ds.reset(); 
  ds.write(0xCC);
  ds.write(0x44);
  delay(750);
  ds.reset();
  ds.write(0xCC);
  ds.write(0xBE);
  data[0] = ds.read(); 
  data[1] = ds.read();
  int Temp = (data[1]<<8)+data[0];
  Temp = Temp>>4;
  return Temp;
 }

void sensorDataout(int idx, float data, int data2){
      Serial.println("Sending data....");
      String string = " { \"idx\" : ";
      string.concat(idx);
      string.concat(", \"nvalue\" : ");
      string.concat(data2);
      string.concat(", \"svalue\" : \"");
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

void relayOut(char relay, char state){  
  Serial.println("relais");
  switch (relay) {
    case '0':  
      switch (state){
        case '0':
        Serial.println("rood uit");
        digitalWrite(rood, LOW);
        alarmOnVol = false;
        blinker = false;
        break;
        case '9':
        Serial.println("rood aan");
        digitalWrite(rood, HIGH);
        alarmOnVol = true;
        blinker = false;
        break;
      }
    break;   
    case '1':  
      switch (state){
        case '0':
        Serial.println("groen uit");
        digitalWrite(groen, LOW);
        alarmOnDeel = false;
        break;
        case '9':
        Serial.println("groen aan");
        digitalWrite(groen, HIGH);
        alarmOnDeel = true;
        break;
      }
    break;
    case '2':  
      switch (state){
        case '0':
        break;
        case '9':
        break;
      }
    break;
    case '3':  
      switch (state){
        case '0':
        break;
        case '9':
        break;
      }
    break;
    case '4':  
      switch (state){
        case '0':
        break;
        case '9':
        break;
      }
    break;      
  }
}

void lightOut(int idx, char cmd[]){  
      Serial.println("lightOut");
      //char data2[] ={"\{\"command\": \"switchlight\", \"idx\": 26, \"switchcmd\": \"Off\", \"level\": 100\}"};
      String string = "\{\"command\": \"switchlight\", \"idx\": ";
      string.concat(idx);
      string.concat(", \"switchcmd\": \"");
      string.concat(cmd);
      string.concat("\", \"level\": 100\} ");
      char data2[80];
      string.toCharArray(data2, 80);
      client.publish("domoticz/in",data2);
      // ... and resubscribe
      client.subscribe("domoticz/arduino"); 
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

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
    }//end switch case
    }//enf first iff
  
  else {
      Serial.println("reject unitId");
      }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
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

