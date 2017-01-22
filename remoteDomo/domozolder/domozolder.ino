// Library's
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "kkino.h"
#define clientId "zolder"

// Network settings
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFA, 0xBB };
IPAddress ip(192, 168, 100, 201);
IPAddress server(192, 168, 100, 54);

//Variables
char unitId = '1'; //Unit id, tbv ontvangst paketten
int idx1 = 24; //IDX number for domoticz
int idx2 = 25;
int idx3 = 26;
int idx4 = 27;
float data1 = 1.0; //Datapoint
float data2 = 1.0;
float data3 = 0.0;
float data4 = 0.0;
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
}

void loop()
{
  if (!client.connected()) {//Reconnect not connected
    reconnect();
  }
  client.loop();

  if ((millis() - last) >= interval){ //To start sending sensordata
    //data1 = bmp.readTemperature();
    //data2 = (bmp.readPressure()/100);
    sensorDataout(idx1, data1);
    sensorDataout(idx2, data2);
    //sensorDataout(idx3, data3);
    //sensorDataout(idx4, data4);
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
    
    if (address == 12509470) {
      switch (onoff) {
        case 0:   
        Serial.print("OFF");
        lightOut(26,"Off"); //idx 26, uit, sturen naar domoticz
        break;
        case 1:
        Serial.print("ON");
        lightOut(26,"On"); //idx 26, aan, sturen naar domoticz
        break;
      }
    }
  }
  
  if ((i<19) && (c!=13)) { 
  inbuf[i++] = c; 
  }
}









