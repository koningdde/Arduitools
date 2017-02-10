// Library's
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS_1 5
#define ONE_WIRE_BUS_2 6

OneWire oneWire_1(ONE_WIRE_BUS_1);
OneWire oneWire_2(ONE_WIRE_BUS_2);

DallasTemperature sensor_1(&oneWire_1);
DallasTemperature sensor_2(&oneWire_2);

#include "kkino.h"
#define clientId "zolder"

#include <dht.h>
dht DHT;
#define DHT11_PIN 2

// Network settings
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFA, 0xBB };
IPAddress ip(192, 168, 100, 201);
IPAddress server(192, 168, 100, 54);

//Variables
char unitId = '1'; //Unit id, tbv ontvangst paketten
int idx1 = 24; //IDX number for domoticz
int idx2 = 25;
int idx3 = 42;
//int idx4 = 39;
float data1; //Datapoint
float data2;
float data3;
float data4;
int relay4 = 9; //Hardwire output
int relay3 = 8; //Hardwire output
int relay1 = 4; //Hardwire output
int relay2 = 6; //Hardwire output

unsigned long last;
unsigned long interval = (600000); //Interval to send sensor data

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
       case 'k':
       kkOut( (char)payload[2],(char)payload[3] );
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

void sensorDataoutint(int idx, int data, int data2){
      Serial.println("Sending data....");
      String string = " { \"idx\" : ";
      string.concat(idx);
      //string.concat(", \"nvalue\" : 0, \"svalue\" : \"");
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

void sensorDataouthum(int idx, int data, int data2){
      Serial.println("Sending data....");
      String string = " { \"idx\" : ";
      string.concat(idx);
      //string.concat(", \"nvalue\" : 0, \"svalue\" : \"");
      string.concat(", \"nvalue\" : ");
      string.concat(1);
      string.concat(", \"svalue\" : \"");
      string.concat(data);
      string.concat(";");
      string.concat(data2);
      string.concat(";0");
      string.concat("\" \}");
      Serial.println(string);
      char output[80];
      string.toCharArray(output, 80);
      //char data2[] ={"\{\"command\": \"switchlight\", \"idx\": 14, \"switchcmd\": \"Off\", \"level\": 100\}"};
      client.publish("domoticz/in",output);
      // ... and resubscribe
      client.subscribe("domoticz/arduino"); 
}

void lightOut(int idx, char cmd[]){  
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

  
void setup()
{
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  Serial.begin(57600);
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
  last = millis();
  kk_init();
  delay(1500); 
  sensor_1.begin();
  sensor_2.begin();
}

void loop()
{
  if (!client.connected()) {//Reconnect not connected
    reconnect();
  }
  client.loop();

  if ((millis() - last) >= interval){ //To start sending sensordata
  
    sensor_1.requestTemperatures();
    sensor_2.requestTemperatures();
    data1 = (sensor_1.getTempCByIndex(0));
    data2 = (sensor_2.getTempCByIndex(0));
    //Serial.println(data1);
    //Serial.println(data2);
    int chk = DHT.read11(DHT11_PIN);
    
    data3 = (DHT.humidity);
    data4 = (DHT.temperature);
    sensorDataout(idx1, data1, 0);
    sensorDataout(idx2, data2, 0);

    
    if ((data3 > -10) && (data3 < 100)) { //hum + temp out
    sensorDataouthum(idx3, data4, (int) data3);
    }
    
    last = millis();   
  }

  
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
    
    if (address == 25711616) {
      switch (onoff) {
        case 0:   
        Serial.print("OFF");
        lightOut(37,"Off"); //idx 26, uit, sturen naar domoticz
        
        break;
        case 1:
        Serial.print("ON");
        lightOut(37,"On"); //idx 26, aan, sturen naar domoticz
        
        break;

      }
    }
  }
  
  if ((i<19) && (c!=13)) { 
  inbuf[i++] = c; 
  }
}


