// Library's
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include "kkino.h"
#define clientId "schuur"

// Network settings
byte mac[]    = {  0x10, 0x20, 0xBA, 0xFE, 0xFA, 0xBB };
IPAddress ip(192, 168, 100, 204);
IPAddress server(192, 168, 100, 54);

//Variables
char unitId = '4'; //Unit id, tbv ontvangst paketten
int idx1 = 99; //IDX number for domoticz
int idx2 = 99;
int idx3 = 99;
//int idx4 = 39;
float data1; //Datapoint
float data2;
float data3;
float data4;
int relay4 = 22; //Hardwire output
int relay3 = 24; //Hardwire output
int relay1 = 26; //Hardwire output
int relay2 = 26; //Hardwire output

unsigned long last;
unsigned long interval = (300000); //Interval to send sensor data

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
  Serial.println("relayOut");
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
    case '4':  
      switch (state){
        case '0':
        kk_send(23886398,0,0,0); //Verlichting kas
        Serial.println("send kk off");
        break;
        case '9':
        kk_send(23886398,0,1,0);
        Serial.println("send kk on");
        break;
      }
      break; 
      case '5':  
      switch (state){
        case '0':
        kk_send(3,2,0,0); //Verlichting kas
        Serial.println("send kk off");
        break;
        case '9':
        kk_send(3,2,1,0);
        Serial.println("send kk on");
        break;
      }
    break; 
    case '6':  
      switch (state){
        case '0':
        kk_send(0,0,0,0); //Verlichting kas
        Serial.println("send kk off");
        break;
        case '9':
        kk_send(0,0,1,0);
        Serial.println("send kk on");
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

void reconnect() {
  // Loop until we're reconnected
  Serial.println("reconnect");
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
}

void loop()
{
  if (!client.connected()) {//Reconnect not connected
    reconnect();
  }
  client.loop();

  if ((millis() - last) >= interval){ //To start sending sensordata
 

    //sensorDataout(idx1, data1, 0);
    //sensorDataout(idx2, data2, 0);
    
    last = millis();   
  }

  
  if (kk_available()) {
    Serial.print("kkReceive");   
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

