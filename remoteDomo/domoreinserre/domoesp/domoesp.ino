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
#include <DallasTemperature.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;
OneWire ds(14); //Dallas op pin
DallasTemperature sensors(&ds);

const char* ssid = "Slangenpiraat";
const char* password = "Hyundai1";
const char* mqtt_server = "192.168.1.101";

#define clientId "Serre"
char unitId = '2'; //Unit id
int idx1 = 314; //IDX number for domoticz
int idx2 = 313; //IDX number for domoticz
float data1 = 0.0; //Datapoint
float data2 = 0;

int relay1 = 15; //Hardwire output
int led = 16;
uint16_t lux;
  
unsigned long last;
unsigned long interval = 300000; //Interval to send sensor data
//unsigned long interval = 3000; //Interval to send sensor data

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(relay1, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
  lightMeter.begin();
}

void loop() {

  if (!client.connected()) {
    digitalWrite(led, LOW);
    reconnect();
    digitalWrite(led, HIGH);
  }
  
  client.loop();

   if ((millis() - last) >= interval){ //To start sending sensordata
    data1 = dallas();
    data2 = 0.0;
    lux = lightMeter.readLightLevel();
    Serial.print(lux);
    sensorDataout(idx1, data1, data2);
    sensorDataout(idx2, lux, 0);
    last = millis();   
  }

  //Read connected sensors

    
}//end main loop

void sensorDataout(int idx, float data, float data2){
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
        //digitalWrite(rood, LOW);
        break;
        case '9':
        //digitalWrite(rood, HIGH);
        break;
      }
    break;   
    case '1':  
      switch (state){
        case '0':
        //digitalWrite(geel, LOW);
        break;
        case '9':
        //digitalWrite(geel, HIGH);
        break;
      }
    break;
    case '2':  
      switch (state){
        case '0':
        //digitalWrite(BLAUW, LOW);
        break;
        case '9':
        //digitalWrite(BLAUW, HIGH);
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
  WiFi.mode(WIFI_STA);
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
  Serial.println();
  WiFi.printDiag(Serial);
  Serial.println();
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

float dallas(){
  sensors.requestTemperatures();
  float Temp = sensors.getTempCByIndex(0);
  return Temp;
 }


